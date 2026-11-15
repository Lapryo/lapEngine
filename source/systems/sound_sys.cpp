#include "systems/sound_sys.hpp"
#include "world.hpp"
#include "elements.hpp"
#include "raymath.h"

#include <algorithm>
#include <cmath>

using namespace lapCore::ELEMENTS::Physics;

static float CalculatePan(Vector2 direction)
{
    float angle = atan2f(direction.y, direction.x);
    return (cosf(angle) + 1.0f) * 0.5f;
}

void SoundSystem::Update(float deltaTime, registry& registry)
{
    if (!IsAudioDeviceReady())
        return;

    auto view = registry.view<SoundPoint>();

    for (auto [object, sound] : view.each())
    {
        if (!sound.active)
        {
            if (!sound.playing)
                continue;

            if (sound.isMusic)
            {
                if (IsMusicValid(sound.musicPlayback))
                    StopMusicStream(sound.musicPlayback);
            }
            else
            {
                if (IsSoundValid(sound.soundPlayback))
                    StopSound(sound.soundPlayback);
            }

            sound.playing = false;

            if (sound.onEnd)
                sound.onEnd();

            continue;
        }

        // ---------------------------------------------------------------------
        // Initialize playback
        // ---------------------------------------------------------------------

        if (!sound.initialized && sound.autoPlay)
        {
            if (sound.isMusic)
            {
                auto* resource = scene->world->resources.music.TryGet(sound.audioID);

                if (!resource)
                    continue;

                sound.musicPlayback = LoadMusicStream(resource->path.c_str());

                if (!IsMusicValid(sound.musicPlayback))
                    continue;

                SetMusicVolume(sound.musicPlayback, 0.f);
                PlayMusicStream(sound.musicPlayback);
                sound.fadeInElapsed = 0.f;

                sound.initialized = true;
                sound.playing = true;

                if (sound.onStart)
                    sound.onStart();
            }
            else
            {
                auto* resource = scene->world->resources.sounds.TryGet(sound.audioID);

                if (!resource)
                    continue;

                if (sound.usesOwn)
                    sound.soundPlayback = LoadSoundAlias(resource->asset);
                else
                    sound.soundPlayback = resource->asset;

                if (!IsSoundValid(sound.soundPlayback))
                    continue;

                // Title theme is a SoundPoint of type Sound, not Music;
                // soften its start too without touching the WAV itself.
                SetSoundVolume(sound.soundPlayback, 0.f);
                PlaySound(sound.soundPlayback);
                sound.fadeInElapsed = 0.f;

                sound.initialized = true;
                sound.playing = true;

                if (sound.onStart)
                    sound.onStart();
            }
        }

        // ---------------------------------------------------------------------
        // Update playback state
        // ---------------------------------------------------------------------

        if (sound.isMusic)
        {
            if (!IsMusicValid(sound.musicPlayback))
                continue;

            UpdateMusicStream(sound.musicPlayback);

            if (IsMusicStreamPlaying(sound.musicPlayback))
            {
                sound.playing = true;
            }
            else if (sound.playing)
            {
                sound.playing = false;

                if (sound.onEnd)
                    sound.onEnd();
            }
        }
        else
        {
            if (!IsSoundValid(sound.soundPlayback))
                continue;

            if (IsSoundPlaying(sound.soundPlayback))
            {
                sound.playing = true;
            }
            else if (sound.playing)
            {
                sound.playing = false;

                if (sound.onEnd)
                    sound.onEnd();
            }
        }

        // ---------------------------------------------------------------------
        // Calculate volume and pan
        // ---------------------------------------------------------------------

        float volume = 1.0f;
        float pan = 0.5f;

        if (sound.positional)
        {
            auto* transform = scene->FindElement<Transform2D>(object);

            if (!transform)
                continue;

            Vector2 direction = Vector2Subtract(
                transform->position,
                sound.target
            );

            float distance = Vector2Length(direction);

            if (sound.cutoffDistance <= 0.0f || distance >= sound.cutoffDistance)
            {
                volume = 0.0f;
            }
            else
            {
                volume = 1.0f - (distance / sound.cutoffDistance);
                volume = clamp(volume, 0.0f, 1.0f);

                if (distance > 0.0f)
                    pan = CalculatePan(direction);
            }
        }

        // SoundPoint music fades in rather than starting at full volume.
        // Independently, sceneMix is faded out by the owning game's transition.
        sound.fadeInElapsed = std::min(0.40f, sound.fadeInElapsed + std::max(0.f, deltaTime));
        volume *= (sound.fadeInElapsed / 0.40f) * std::clamp(sound.sceneMix, 0.f, 1.f);

        // ---------------------------------------------------------------------
        // Update volume and pan
        // ---------------------------------------------------------------------

        if (sound.isMusic)
        {
            if (!IsMusicValid(sound.musicPlayback))
                continue;

            SetMusicVolume(sound.musicPlayback, volume);
            SetMusicPan(sound.musicPlayback, pan);

            if (sound.playing && sound.onPlay)
                sound.onPlay();
        }
        else
        {
            if (!IsSoundValid(sound.soundPlayback))
                continue;

            SetSoundVolume(sound.soundPlayback, volume);
            SetSoundPan(sound.soundPlayback, pan);

            if (sound.playing && sound.onPlay)
                sound.onPlay();
        }
    }
}
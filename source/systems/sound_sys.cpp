#include "core.hpp"
#include "systems/sound_sys.hpp"

#include <algorithm>
#include <cmath>

static float CalculatePan(Vector2 direction)
{
    float angle = atan2f(direction.y, direction.x);
    return (cosf(angle) + 1.0f) * 0.5f;
}

void SoundSystem::StopAll()
{
    if (!IsAudioDeviceReady())
        return;

    auto view = scene->objects.view<SoundPoint>();

    for (auto [object, sound] : view.each())
    {
        if (sound.isMusic)
        {
            if (IsMusicValid(sound.musicPlayback))
            {
                StopMusicStream(sound.musicPlayback);
                UnloadMusicStream(sound.musicPlayback);
                sound.musicPlayback = {};
            }
        }
        else
        {
            if (IsSoundValid(sound.soundPlayback))
            {
                StopSound(sound.soundPlayback);

                if (sound.usesOwn)
                    UnloadSoundAlias(sound.soundPlayback);

                sound.soundPlayback = {};
            }
        }

        sound.playing = false;
        sound.initialized = false;
    }
}

void SoundSystem::Update(float deltaTime, entt::registry& registry)
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

                PlayMusicStream(sound.musicPlayback);

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

                PlaySound(sound.soundPlayback);

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
                volume = std::clamp(volume, 0.0f, 1.0f);

                if (distance > 0.0f)
                    pan = CalculatePan(direction);
            }
        }

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
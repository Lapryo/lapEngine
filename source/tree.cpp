#include "tree.hpp"

namespace lapCore
{
    template <typename T, typename Identifier, typename Value>
    vector<Identifier> KVTree<T, Identifier, Value>::GatherIdentifiers(vector<Node *> nodes)
    {
        vector<Identifier> identifiers;
        for (const auto& node : nodes)
        {
            identifiers.push_back(node->key);
        }

        return identifiers;
    }

    template <typename T, typename Identifier, typename Value>
    vector<Value> KVTree<T, Identifier, Value>::GatherValues(vector<Node *> nodes)
    {
        vector<Value> values;
        for (const auto& node : nodes)
        {
            values.push_back(node->value);
        }

        return values;
    }

    template <typename T, typename Identifier, typename Value>
    vector<T> KVTree<T, Identifier, Value>::GatherData(vector<Node *> nodes)
    {
        vector<T> data;
        for (const auto& node : nodes)
        {
            data.push_back(node->data);
        }

        return data;
    }

    template<typename T, typename Identifier, typename Value>
    int KVTree<T, Identifier, Value>::DistanceFromRoot(Node * node)
    {
        int distance = 0;
        while (node && node != root)
        {
            node = node->parent;
            distance++;
        }
        return distance;
    }

    template <typename T, typename Identifier, typename Value>
    vector<KVNode<T, Identifier, Value> *> KVTree<T, Identifier, Value>::GetNodesClosestToRoot(vector<Identifier> objects)
    {
        vector<Node*> closestNodes = nullptr;
        int closestDistance = INT_MAX;

        for (const auto& object : objects)
        {
            auto node = GetNode(object);
            if (node)
            {
                int distance = DistanceFromRoot(node);
                if (distance < closestDistance)
                {
                    closestDistance = distance;
                    closestNodes.clear();
                    closestNodes.push_back(node);
                }
                else if (distance == closestDistance)
                {
                    closestNodes.push_back(node);
                }
            }
        }

        return closestNodes;
    }

    template <typename T, typename Identifier, typename Value>
    vector<KVNode<T, Identifier, Value> *> KVTree<T, Identifier, Value>::GetNodes()
    {
        return nodes;
    }

    template <typename T, typename Identifier, typename Value>
    vector<KVNode<T, Identifier, Value> *> KVTree<T, Identifier, Value>::FindNodes(Value id)
    {
        auto it = nameLookup.find(id);
        if (it != nameLookup.end())
        {
            return it->second;
        }

        return vector<KVNode<T, Identifier, Value> *>();
    }

    template <typename T, typename Identifier, typename Value>
    vector<KVNode<T, Identifier, Value> *> KVTree<T, Identifier, Value>::GetChildren(Node *parent)
    {
        auto it = childLookup.find(parent);
        if (it != childLookup.end())
        {
            return it->second;
        }

        return vector<KVNode<T, Identifier, Value> *>();
    }

    template <typename T, typename Identifier, typename Value>
    vector<KVNode<T, Identifier, Value> *> KVTree<T, Identifier, Value>::FindChildren(Node *parent, Value id)
    {
        vector<KVNode<T, Identifier, Value> *> result;
        auto it = childLookup.find(parent);
        if (it != childLookup.end())
        {
            for (const auto& node : it->second)
            {
                if (node->value == id)
                {
                    result.push_back(node);
                }
            }
        }

        return result;
    }

    template <typename T, typename Identifier, typename Value>
    KVNode<T, Identifier, Value> *KVTree<T, Identifier, Value>::GetNode(Identifier object)
    {
        auto it = nodes.find(object);
        if (it != nodes.end())
        {
            return it->second.get();
        }

        return nullptr;
    }
    
    template <typename T, typename Identifier, typename Value>
    KVNode<T, Identifier, Value> *KVTree<T, Identifier, Value>::AddNode(Identifier object, Value id)
    {
        return AddNode(nullptr, object, id);
    }

    template <typename T, typename Identifier, typename Value>
    KVNode<T, Identifier, Value> *KVTree<T, Identifier, Value>::AddNode(Node *parent, Identifier object, Value id)
    {
        auto node = make_unique<KVNode<T, Identifier, Value>>(object, id);
        auto ptr = node.get();
        nodes[object] = std::move(node);

        nameLookup[id].push_back(ptr);
        if (parent)
        {
            childLookup[parent].push_back(ptr);
        }
        else
        {
            childLookup[root].push_back(ptr);
        }

        return ptr;
    }

    template <typename T, typename Identifier, typename Value>
    bool KVTree<T, Identifier, Value>::RemoveNode(Identifier object)
    {
        // Get the parent node first
        Node *parent = nullptr;
        auto node = GetNode(object);
        if (!node || !node->parent) return false;

        parent = node->parent;

        nameLookup.erase(object);
        nodes.erase(object);
        childLookup[parent].erase(object);

        return true;
    }

    template <typename T, typename Identifier, typename Value>
    bool KVTree<T, Identifier, Value>::ReparentNode(Identifier object, Identifier newParent)
    {
        auto node = GetNode(object);
        if (!node || !node->parent) return false;

        auto newParentNode = GetNode(newParent);
        if (!newParentNode) return false;

        // Remove the node from its current parent's list of children
        if (node->parent) {
            childLookup[node->parent].erase(object);
        }

        // Add the node to the new parent's list of children
        childLookup[newParentNode].push_back(node);

        // Update the node's parent pointer
        node->parent = newParentNode;

        return true;
    }

    template <typename T, typename Identifier, typename Value>
    bool KVTree<T, Identifier, Value>::RemoveAndReparentDescendants(Identifier object, Identifier newParent)
    {
        auto node = GetNode(object);
        if (!node) return false;

        // Get the children of the node and move them to the new parent
        // The rest of the descendants will automatically be adjusted due to the awesomeness of pointers
        auto children = childLookup[node];
        for (auto child : children) {
            ReparentNode(child->id, newParent);
        }

        // Remove the node
        RemoveNode(object);

        return true;
    }
}
#pragma once

#include <vector>
#include <unordered_map>
#include <memory>

namespace lapCore
{
    /*
        T acts as the data to store,
        Identifier acts as the unique identifier,
        Value acts as the value that can point to multiple different unique identifiers
    */

    using namespace std;

    template <typename T, typename Identifier, typename Value>
    struct KVTree;

    template <typename T, typename Identifier, typename Value>
    struct KVNode
    {
        using Node = KVNode<T, Identifier, Value>;

        T data;

        Identifier key;
        Value value;
        int index = -2; // -1 means to the end of the children list, 0 means to the front

        KVTree<T, Identifier, Value>* tree = nullptr;

        Node* parent = nullptr;
        vector<Node*> children;

        vector<Node*> previousSiblings, nextSiblings;

        KVNode(KVTree<T, Identifier, Value>* tree, T data);
    };

    template <typename T, typename Identifier, typename Value>
    struct KVTree
    {
        using Node = KVNode<T, Identifier, Value>;

        Node* root = nullptr;

        // Tools
        vector<Identifier> GatherIdentifiers(vector<Node*> nodes);
        vector<Value> GatherValues(vector<Node*> nodes);
        vector<T> GatherData(vector<Node*> nodes);

        int DistanceFromRoot(Node* node);
        vector<Node*> GetNodesClosestToRoot(vector<Identifier> objects);

        // Getters
        vector<Node*> GetNodes();

        vector<Node*> FindNodes(Value id);
        vector<Node*> GetChildren(Node* parent);
        vector<Node*> FindChildren(Node* parent, Value id);

        Node* GetNode(Identifier object);

        // Adders
        Node* AddNode(Identifier object, Value id);
        Node* AddNode(Node* parent, Identifier object, Value id);

        // Removers
        bool RemoveNode(Identifier object);
        bool ReparentNode(Identifier object, Identifier newParent);
        bool RemoveAndReparentDescendants(Identifier object, Identifier newParent);

    private:
        unordered_map<Identifier, unique_ptr<Node>> nodes;

        unordered_map<Identifier, vector<Node*>> nameLookup;
        unordered_map<Node*, unordered_map<Identifier, vector<Node*>>> childLookup;
    };
}
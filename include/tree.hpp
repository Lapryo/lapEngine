#pragma once

namespace lapCore
{
    template <typename T, typename TID>
    struct ObjectTree;

    template <typename T, typename TID>
    struct ObjectTreeNode
    {
        T data;

        ObjectTree<T, TID>* tree = nullptr;

        ObjectTreeNode<T, TID>* parent = nullptr;
        ObjectTreeNode<T, TID>* firstChild = nullptr;
        ObjectTreeNode<T, TID>* lastChild = nullptr;

        ObjectTreeNode<T, TID>* nextSibling = nullptr;
        ObjectTreeNode<T, TID>* prevSibling = nullptr;

        ObjectTreeNode(ObjectTree<T, TID>* tree, T data);
    };

    template <typename T, typename TID>
    struct ObjectTree
    {
        ObjectTreeNode<T, TID>* root = nullptr;

        // Getters
        span<ObjectTreeNode<T, TID>*> GetNodes();
        span<ObjectTreeNode<T, TID>*> GetChildren(ObjectTreeNode<T, TID>* parent, TID id);
        ObjectTreeNode<T, TID>* GetNode(Object object);
        ObjectTreeNode<T, TID>* GetFirstChild(ObjectTreeNode<T, TID>* parent, TID id);

        // Adders
        T AddNode(TID id);
        T AddNode(ObjectTreeNode<T, TID>* parent, TID id);

        // Removers
        bool RemoveNode(T object);
        bool ReparentNode(T object, T newParent);
        bool RemoveAndReparentDescendants(T object, T newParent);

    private:
        unordered_map<T, unique_ptr<ObjectTreeNode<T, TID>>> nodes;

        unordered_map<TID, vector<ObjectTreeNode<T, TID>*>> nameLookup;
        unordered_map<ObjectTreeNode<T, TID>*, unordered_map<TID, vector<ObjectTreeNode<T, TID>*>>> childLookup;
    };
}
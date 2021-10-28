#pragma once

#include <types/scene.h>

#include <components/GeometryEvaluator.h>

enum class ItemState {
    created, clean, dirty, deleted
};

template <typename T>
struct ItemPoolBehavior {
    // TODO: manipulation with individual geometries will be provided by interface to ensure keeping track of
    
    virtual T& newItem()
    {
        uint32 index = items.size();
        items.push_back(T());
        itemsStates.push_back(ItemState::created);
        return items[index];
    }
    
    virtual void addItems(std::vector<T> items)
    {
        this->items.insert(this->items.end(), items.begin(), items.end());
        itemsStates.insert(itemsStates.end(), items.size(), ItemState::created);
    }
    
    virtual T& getItem(uint32 index)
    {
        itemsStates[index] = ItemState::dirty;
        return items[index];
    }
    
    inline const T& readItem(uint32 index) const
    {
        return items[index];
    }
    
    protected:
        std::vector<T> items;
        std::vector<ItemState> itemsStates;
};

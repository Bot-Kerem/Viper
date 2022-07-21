#pragma once
#include <list>
#include <typeindex>
#include <map>
#include "../event/event.hpp"
#include "../basefunctionhandler/basefunctionhandler.hpp"
#include "../memberfunctionhandler/memberfunctionhandler.hpp"

namespace Viper::Events {

   typedef std::list<HandlerFunctionBase*> HandlerList;
    class EventBus {
    public:
        template<typename EventType>
        void Commit(EventType * E) {
            HandlerList * Handlers = subscribers[typeid(EventType)];

            if (Handlers == nullptr) {
                return;
            }

            for (auto & Handler : *Handlers) {
                if (Handler != nullptr) {
                    Handler->Exec(E);
                }
            }
        }

        template<class T, class EventType>
        void Subscribe(T * instance, void (T::*memberFunction)(EventType *)) {
            HandlerList * Handlers = subscribers[typeid(EventType)];

            //First time initialization
            if (Handlers == nullptr) {
                Handlers = new HandlerList();
                subscribers[typeid(EventType)] = Handlers;
            }

            Handlers->push_back(new MemberFunctionHandler<T, EventType>(instance, memberFunction));
        }
    private:
        std::map<std::type_index, HandlerList*> subscribers;
    };

};
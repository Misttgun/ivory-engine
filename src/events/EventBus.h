#pragma once

#include <functional>
#include <list>
#include <map>
#include <memory>
#include <typeindex>
#include "Event.h"

class IEventCallback
{
private:
	virtual void Call(Event& e) = 0;

public:
	virtual ~IEventCallback() = default;

	void Execute(Event& e)
	{
		Call(e);
	}
};

template <typename TOwner, typename TEvent>
class EventCallback : public IEventCallback
{
private:
	typedef void (TOwner::* CallbackFunction)(TEvent&);

	TOwner* m_ownerInstance;
	CallbackFunction m_callbackFunction;

	void Call(Event& e) override
	{
		std::invoke(m_callbackFunction, m_ownerInstance, static_cast<TEvent&>(e));
	}

public:
	EventCallback(TOwner* ownerInstance, const CallbackFunction callbackFunction) : m_ownerInstance(ownerInstance), m_callbackFunction(callbackFunction)
	{}

	~EventCallback() override = default;
};

typedef std::list<std::unique_ptr<IEventCallback>> HandlerList;

class EventBus
{
private:
	std::map<std::type_index, std::unique_ptr<HandlerList>> m_subscribers;

public:
	EventBus() = default;
	~EventBus()
	{
		Reset();
	}

	void Reset()
	{
		m_subscribers.clear();
	}

	/////////////////////////////////////////////////////////////////////// 
	// Subscribe to an event type <T>
	// In our implementation, a listener subscribes to an event
	// Example: eventBus->SubscribeToEvent<CollisionEvent>(this, &Game::onCollision);
	/////////////////////////////////////////////////////////////////////// 
	template <typename TEvent, typename TOwner>
	void SubscribeToEvent(TOwner* ownerInstance, void (TOwner::* callbackFunction)(TEvent&))
	{
		if(!m_subscribers[typeid(TEvent)].get())
		{
			m_subscribers[typeid(TEvent)] = std::make_unique<HandlerList>();
		}
		auto subscriber = std::make_unique<EventCallback<TOwner, TEvent>>(ownerInstance, callbackFunction);
		m_subscribers[typeid(TEvent)]->push_back(std::move(subscriber));
	}

	/////////////////////////////////////////////////////////////////////// 
	// Emit an event of type <T>
	// In our implementation, as soon as something emits an
	// event we go ahead and execute all the listener callback functions
	// Example: eventBus->EmitEvent<CollisionEvent>(player, enemy);
	/////////////////////////////////////////////////////////////////////// 
	template <typename TEvent, typename ...TArgs>
	void EmitEvent(TArgs&& ...args)
	{
		const auto handlers = m_subscribers[typeid(TEvent)].get();
		if(handlers)
		{
			TEvent event(std::forward<TArgs>(args)...);
			for(auto it = handlers->begin(); it != handlers->end(); ++it)
			{
				auto handler = it->get();
				handler->Execute(event);
			}
		}
	}
};

#pragma once

#include <typeinfo>

namespace smart_ptr
{
	namespace details
	{
		template <class T>
		void simple_swap(T& first, T& second)
		{
			T temp = first;
			first = second;
			second = temp;
		}
		
		
		struct linked
		{
			explicit linked()
			:next(nullptr)
			,prev(nullptr) {}
			
			explicit linked(linked* next, linked* prev)
			:next(next)
			,prev(prev) {}
			
			bool linked_unique() const
			{
				if ((next == nullptr) && (prev == nullptr))
					return true;
				else
					return false;
			}
			
			void insert_after(linked const & from)
			{
				next = from.next;
				if (next != nullptr)
					next->prev = this;
				
				prev = (linked*)&from;
				from.setNext(this);
			}
			
			void swap_linked(const linked & another)
			{			
				simple_swap(next, another.next);
				simple_swap(prev, another.prev);
				update_links();
				another.update_links();
			}
			
			~linked()
			{
				if (next != nullptr)
					next->prev = prev;
				
				if (prev != nullptr)
					prev->next = next;
			}
			
		private:
			mutable linked* next;
			mutable linked* prev;
			
			void setNext(linked* next) const
			{
				this->next = next;
			}
			void setPrev(linked* prev) const
			{
				this->prev = prev;
			}
			
			void update_links() const
			{
				if (next != nullptr)
				{
					next->prev = (linked*)this;
				}
			
				if (prev != nullptr)
				{
					prev->next = (linked*)this;
				}
			}
		};
	} //details
	
	
	template<class T>
	struct linked_ptr : private details::linked
	{
		T& operator*() const
		{
			return *p_;
		}
		T* operator->() const
		{
			return p_;
		}
		T* get() const
		{
			return p_;
		}
		
		bool unique() const
		{
			if (p_ != nullptr && linked_unique())
				return true;
			else
				return false;
		}
		
		linked_ptr()
			:linked()
			,p_(nullptr)
		{}
		
		explicit linked_ptr(T* p)
			:linked()
			,p_(p)
		{}
		
		linked_ptr(const linked_ptr & from)
			:linked()
			,p_(from.get())
		{
			insert_after(from);
		}
		
		linked_ptr& operator=(const linked_ptr & from)
		{
			if (*this == from)
				return *this;
			linked_ptr temp(from);
			this->swap(temp);
			return *this;
		}

		template <class U>
		linked_ptr(linked_ptr<U> const & from)
			:linked()
			,p_(from.get())
		{
			insert_after(from);
		}
		
		
		
		template <class U>
		void swap(linked_ptr<U>& another)
		{
			if (*this == another)
				return;
			swap_linked(another);			
			details::simple_swap(p_, another.p_);
		}
		
		
		template <class U>
		linked_ptr& operator=(const linked_ptr<U>& from)
		{
			if (*this == from)
				return *this;
			linked_ptr temp(from);
			this->swap(temp);
			return *this;
		}
		
		void reset()
		{
			linked_ptr temp;
			this->swap(temp);
		}
		
		void reset(T* p)
		{
			linked_ptr temp(p);
			this->swap(temp);
		}
		
		~linked_ptr()
		{
			if (linked_unique())
			{
				delete p_;
			}
			(void)sizeof(T);
		}
		
		explicit operator bool() const
		{
          return (p_ != nullptr);
    	}
		

	private:
		mutable T * p_;
		
		template< class >
   		friend struct linked_ptr;
	};
	
	
	template <class T, class U>
	bool operator==(const linked_ptr<T>& first, const linked_ptr<U>& second)
	{
		return first.get() == second.get();
	}
	
	template <class T, class U>
	bool operator!=(const linked_ptr<T>& first, const linked_ptr<U>& second)
	{
		return !(first == second);
	}
	
	template <class T, class U>
	bool operator<(const linked_ptr<T>& first, const linked_ptr<U>& second)
	{
		return first.get() < second.get();
	}
	
	template <class T, class U>
	bool operator<=(const linked_ptr<T>& first, const linked_ptr<U>& second)
	{
		return ((first == second) || (first < second));
	}
	
	template <class T, class U>
	bool operator>(const linked_ptr<T>& first, const linked_ptr<U>& second)
	{
		return !(first <= second);
	}
	
	template <class T, class U>
	bool operator>=(const linked_ptr<T>& first, const linked_ptr<U>& second)
	{
		return !(first < second);
	}
	
} //smart_ptr

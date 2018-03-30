#ifndef FN_HPP
#define FN_HPP

#include <memory>
#include <type_traits>
#include <typeinfo>
#include <utility>

namespace fn
{
    class bad_function_call : public std::exception
    {
        virtual const char* what() const noexcept override
        {
            return "Try to apply empty function";
        }
    };


    template <typename UnusedType>
    class function;

    template<typename RETURN_TYPE, typename... ARG_TYPE>
    class function <RETURN_TYPE (ARG_TYPE...)>
    {
    public:
        function()
            : functionHolder()
            , freeFunction(nullptr) {}

        function(std::nullptr_t)
            : functionHolder()
            , freeFunction(nullptr) {}

        function(const function & other)
            : freeFunction(other.freeFunction)
        { 
            if (freeFunction == nullptr && other.functionHolder)
            {
                functionHolder = std::shared_ptr<BaseFunctionHolder>(other.functionHolder->clone());    
            }
        }


        void swap(function & other)
        {
            std::swap(freeFunction, other.freeFunction);
            functionHolder.swap(other.functionHolder);
        }

        function(function && other)
            : freeFunction(other.freeFunction)
        {
            if (freeFunction == nullptr && other.functionHolder)
            {
                functionHolder = other.functionHolder;
                other.functionHolder.reset(static_cast<BaseFunctionHolder*>(nullptr));
            }
        }

        function & operator=(const function & other)
        {
            if (&other == this)
                return *this;

            function new_copy(other);
            swap(new_copy);
            return *this;
        }

        function & operator=(function && other)
        {
            if (&other == this)
                return *this;
            
            function new_copy(std::move(other));
            swap(new_copy);
            
            return *this;
        }
        
        function(RETURN_TYPE(*f)(ARG_TYPE...))
            : freeFunction(f) { }

        template<typename T>
        function(T f)
            : functionHolder(std::make_shared<FunctionHolder<T> >(f))
            , freeFunction(nullptr) { }

        explicit operator bool() const {
            if (freeFunction != nullptr) {
                return true;
            }
            else
            {
                return functionHolder.operator bool();
            }
            
            return (freeFunction != nullptr) || functionHolder.operator bool();
        }

        RETURN_TYPE operator()(ARG_TYPE && ... arg) const
        {
            if (freeFunction != nullptr) {
                return freeFunction(std::forward<ARG_TYPE>(arg) ...);
            }
            else
            {
                if (!functionHolder)
                {
                    throw bad_function_call();
                }
                return functionHolder->operator()(std::forward<ARG_TYPE>(arg) ...);
            }
        }


    private:
        class BaseFunctionHolder {
        public:
            virtual ~BaseFunctionHolder() {}
            virtual RETURN_TYPE operator()(ARG_TYPE && ... arg) = 0;
            virtual BaseFunctionHolder* clone() const = 0;
        };

        template<typename FUNCTION_TYPE>
        class FunctionHolder : public BaseFunctionHolder {
        public:
            FUNCTION_TYPE function_object;
            
            FunctionHolder(FUNCTION_TYPE f)
                : function_object(f) {}

            virtual ~FunctionHolder() {}
            
            virtual RETURN_TYPE operator()(ARG_TYPE && ... arg) override
            {
                return function_object(std::forward<ARG_TYPE>(arg) ...);
            }

            virtual BaseFunctionHolder* clone() const override
            {
                return new FunctionHolder(function_object);
            }
        };

        std::shared_ptr<BaseFunctionHolder> functionHolder;
        using FreeFunctionType = RETURN_TYPE(*)(ARG_TYPE...);
        FreeFunctionType freeFunction;
    };

    template<typename RETURN_TYPE, typename... ARG_TYPE>
    void swap(function<RETURN_TYPE(ARG_TYPE...)> & function_first, function<RETURN_TYPE(ARG_TYPE...)> & function_second)
    {
        function_first.swap(function_second);
    }

} // fn

#endif // FN_HPP


//
//  File: DKFunction.h
//  Author: Hongtae Kim (tiff2766@gmail.com)
//
//  Copyright (c) 2004-2017 Hongtae Kim. All rights reserved.
//

#pragma once
#include "../DKInclude.h"
#include "DKTuple.h"
#include "DKTypeTraits.h"
#include "DKInvocation.h"
#include "DKObject.h"
#include "DKFixedSizeAllocator.h"

namespace DKFoundation
{
	template <typename Function> struct DKFunctionSignature;
	/**
	 @brief
	 A prototype of function object. template parameter must be function type.
	  (ex: DKFunctionSignature<void (int)> for 'void func(int)' )
	 This class is able to generate DKOperation, DKInvocation object.
	 It is very useful to implement script system or reflection features.

	 The purpose of this class is to provide common type of Function-object.

	 You can create DKFunctionSignature object with following ways.
	  - DKFunction( with function pointer )
	  - DKFunction( with function object or lambda )
	  - DKFunction( an object reference or pointer, member function pointer )

	 Example:
	 @code
	   void func(int) {..}

	   // generate DKFunctionSignature object from function.
	   auto sig = DKFunction(&func);
	   sig->Invoke(3);  // direct call

	   // generate operation object.
	   auto op = sig->Invocation(3);
	   sig = NULL;  // delete sig object. but 'op' is still alive.
	   op->Perform();  // call func.

	   // you can generate from lambda also.
	   auto sig = DKFunction( [](int) { ... } );
	   sig->Invoke(3);

	   // generate from class object.
	   auto sig = DKFunction(MyObject, &MyClass::Function);

	   // create object on the fly
	   auto sig = DKFunction(AClass(), &AClass::Function);
	 @endcode

	 @note
	  You can create DKOperation object from DKFunctionSignature
	  and you can store that into array or list for later use.

	 @note
	  DKFunctionSignature< function-type > template parameters affects only
	  function parameters. function and member function are same type.\n
	   - void func(int) ==> DKFunctionSignature<void (int)>			\n
	   - void AClass::Func(int) ==> DKFunctionSignature<void (int)>
	 */
	template <typename R, typename... Ps> struct DKFunctionSignature<R (Ps...)>
	{
		using ParameterNumber = DKNumber<sizeof...(Ps)>;
		using ParameterTuple = DKTuple<Ps...>;
		using ReturnType = R;

		/// direct call
		virtual ReturnType Invoke(Ps...) const = 0;
		virtual ReturnType InvokeWithTuple(ParameterTuple&) const = 0;

		/// indirect-invocation call
		virtual DKObject<DKInvocation<R>> Invocation(Ps...) const = 0;
		virtual DKObject<DKInvocation<R>> InvocationWithTuple(ParameterTuple&) const = 0;
		virtual DKObject<DKInvocation<R>> InvocationWithTuple(ParameterTuple&&) const = 0;

		virtual ~DKFunctionSignature() {}

		template <typename... Ts> constexpr static bool CanInvokeWithParameterTypes()
		{
			return DKTypeList<Ts...>::template IsConvertible<typename ParameterTuple::TypeList>();
		};
	};

	namespace Private
	{
		template <typename T, bool SmallSize = (sizeof(T) < 16) > struct FunctionAllocator;
		template <typename T> struct FunctionAllocator<T, true> // custom allocator for small size
		{
			static DKAllocator& Instance()
			{
				return DKFixedSizeAllocator<sizeof(T), 4, 1024>::AllocatorInstance();
			}
		};
		template <typename T> struct FunctionAllocator<T, false> // default allocator
		{
			static DKAllocator& Instance()
			{
				return DKAllocator::DefaultAllocator();
			}
		};

		////////////////////////////////////////////////////////////////////////////////
		// use tuple as function arguments (variadic templates)
		template <int N, int Count, int... Ns> struct _TupleInvoker
		{
			using Result = typename _TupleInvoker<N+1, Count-1, Ns..., N>::Result;
		};
		template <int N, int... Ns> struct _TupleInvoker<N, 0, Ns...>
		{
			struct _Result
			{
				// regular function pointer or function object (functor).
				template <typename Ret, typename Func, typename Tuple>
				static auto Invoke(Func&& fn, Tuple&& tuple)->Ret
				{
					return fn( tuple.template Value<Ns>()... );
				}
				// class member function. (using class object as pointer)
				template <typename Ret, class Obj, typename Func, typename Tuple>
				static auto Invoke(Obj&& obj, Func fn, Tuple&& tuple)->Ret
				{
					return (obj.*fn)( tuple.template Value<Ns>()... );
				}
			};
			using Result = _Result;
		};
		template <int Length> using TupleInvoker = typename _TupleInvoker<0, Length>::Result;

		////////////////////////////////////////////////////////////////////////////////
		// generate Invocation object from regular function, function object.
		// implementation of DKFunctionSignature. (sublcass)
		template <typename Function, typename R, typename... Ps> class FunctionObjectInvoker
		: public DKFunctionSignature<R (Ps...)>
		{
		public:
			using Signature = DKFunctionSignature<R (Ps...)>;
			using ReturnType = typename Signature::ReturnType;
			using ParameterTuple = typename Signature::ParameterTuple;

			struct _Invocation : public DKInvocation<R>
			{
				_Invocation(Function& f) : function(f) {}
				_Invocation(Function& f, ParameterTuple& t) : function(f), tuple(t) {}
				_Invocation(Function& f, ParameterTuple&& t) : function(f), tuple(static_cast<ParameterTuple&&>(t)) {}
				Function function;
				ParameterTuple tuple;
				R Invoke() const override
				{
					Function& fn = const_cast<_Invocation&>(*this).function;
					ParameterTuple& tup = const_cast<_Invocation&>(*this).tuple;
					return TupleInvoker<ParameterTuple::Length>::template Invoke<R>(fn, tup);
				}
			};

			ReturnType Invoke(Ps... vs) const override
			{
				Function& fn = const_cast<FunctionObjectInvoker&>(*this).function;
				return fn(std::forward<Ps>(vs)...);
			}
			ReturnType InvokeWithTuple(ParameterTuple& tuple) const override
			{
				Function& fn = const_cast<FunctionObjectInvoker&>(*this).function;
				return TupleInvoker<ParameterTuple::Length>::template Invoke<ReturnType>(fn, tuple);
			}
			DKObject<DKInvocation<R>> Invocation(Ps... vs) const override
			{
				Function& fn = const_cast<FunctionObjectInvoker&>(*this).function;
				DKAllocator& alloc = FunctionAllocator<_Invocation>::Instance();
				return new(alloc) _Invocation(fn, ParameterTuple(DKTupleValueSet(), std::forward<Ps>(vs)...));
			}
			DKObject<DKInvocation<R>> InvocationWithTuple(ParameterTuple& tuple) const override
			{
				Function& fn = const_cast<FunctionObjectInvoker&>(*this).function;
				DKAllocator& alloc = FunctionAllocator<_Invocation>::Instance();
				return new(alloc) _Invocation(fn, tuple);
			}
			DKObject<DKInvocation<R>> InvocationWithTuple(ParameterTuple&& tuple) const override
			{
				Function& fn = const_cast<FunctionObjectInvoker&>(*this).function;
				DKAllocator& alloc = FunctionAllocator<_Invocation>::Instance();
				return new(alloc) _Invocation(fn, static_cast<ParameterTuple&&>(tuple));
			}

			FunctionObjectInvoker(Function& fn) : function(fn) {}
			FunctionObjectInvoker(Function&& fn) : function(static_cast<Function&&>(fn)) {}
			Function function;
		};
		////////////////////////////////////////////////////////////////////////////////
		// generate Invocation object from class member function.
		// implementation of DKFunctionSignature. (sublcass)
		template <class Object, typename Function, typename R, typename... Ps> class FunctionMemberObjectInvoker
		: public DKFunctionSignature<R (Ps...)>
		{
		public:
			using Signature = DKFunctionSignature<R (Ps...)>;
			using ReturnType = typename Signature::ReturnType;
			using ParameterTuple = typename Signature::ParameterTuple;

			struct _Invocation : public DKInvocation<R>
			{
				_Invocation(Object& obj, Function f) : object(obj), function(f) {}
				_Invocation(Object& obj, Function f, ParameterTuple& t) : object(obj), function(f), tuple(t) {}
				_Invocation(Object& obj, Function f, ParameterTuple&& t) : object(obj), function(f), tuple(static_cast<ParameterTuple&&>(t)) {}
				Object object;
				Function function;
				ParameterTuple tuple;
				R Invoke() const override
				{
					Object& obj = const_cast<_Invocation&>(*this).object;
					ParameterTuple& tup = const_cast<_Invocation&>(*this).tuple;
					return TupleInvoker<ParameterTuple::Length>::template Invoke<R>(*obj, function, tup);
				}
			};

			ReturnType Invoke(Ps... vs) const override
			{
				Object& obj = const_cast<FunctionMemberObjectInvoker&>(*this).object;
				return ((*obj).*function)(std::forward<Ps>(vs)...);
			}
			ReturnType InvokeWithTuple(ParameterTuple& tuple) const override
			{
				Object& obj = const_cast<FunctionMemberObjectInvoker&>(*this).object;
				return TupleInvoker<ParameterTuple::Length>::template Invoke<ReturnType>(*obj, function, tuple);
			}
			DKObject<DKInvocation<R>> Invocation(Ps... vs) const override
			{
				Object& obj = const_cast<FunctionMemberObjectInvoker&>(*this).object;
				DKAllocator& alloc = FunctionAllocator<_Invocation>::Instance();
				return new(alloc) _Invocation(obj, function, ParameterTuple(DKTupleValueSet(), std::forward<Ps>(vs)...));
			}
			DKObject<DKInvocation<R>> InvocationWithTuple(ParameterTuple& tuple) const override
			{
				Object& obj = const_cast<FunctionMemberObjectInvoker&>(*this).object;
				DKAllocator& alloc = FunctionAllocator<_Invocation>::Instance();
				return new(alloc) _Invocation(obj, function, tuple);
			}
			DKObject<DKInvocation<R>> InvocationWithTuple(ParameterTuple&& tuple) const override
			{
				Object& obj = const_cast<FunctionMemberObjectInvoker&>(*this).object;
				DKAllocator& alloc = FunctionAllocator<_Invocation>::Instance();
				return new(alloc) _Invocation(obj, function, static_cast<ParameterTuple&&>(tuple));
			}

			using ObjectTraits = DKTypeTraits<Object>;
			// Object type should be one of T*, const T*, DKObject<T>, Wrapper<T>, Wrapper<const T>
			// reference type cannot be used.
			static_assert(ObjectTraits::IsReference == 0, "Object must not be a reference type!");

			using UnqualifedObjectType = typename ObjectTraits::UnqualifiedReferredType;
			FunctionMemberObjectInvoker(UnqualifedObjectType& obj, Function fn) : object(obj), function(fn) {}
			FunctionMemberObjectInvoker(UnqualifedObjectType&& obj, Function fn) : object(obj), function(fn) {}
			FunctionMemberObjectInvoker(const UnqualifedObjectType& obj, Function fn) : object(obj), function(fn) {}

			Object object;
			Function function;
		};

		////////////////////////////////////////////////////////////////////////////////
		// validate regular function or function object callable.
		template <typename T> struct FunctionTypeSelector // function object.
		{
			template <typename U> static DKTrue HasFunctionOperator(decltype(&U::operator()));
			template <typename U> static DKFalse HasFunctionOperator(...);
			enum {IsFunctionObject = decltype(HasFunctionOperator<T>(0))::Value};

			static_assert( IsFunctionObject, "Type is not function object!");

			using FunctionTraits = DKMemberFunctionPointerTraits<decltype(&T::operator())>;
			using ReturnType = typename FunctionTraits::ReturnType;
			using ParameterTypeList = typename FunctionTraits::ParameterTypeList;
			template <typename... Ts> using _Invoker = FunctionObjectInvoker<T, ReturnType, Ts...>;
			using Invoker = typename ParameterTypeList::template TypesInto<_Invoker>;
			using Signature = typename Invoker::Signature;
		};
		template <typename T> struct FunctionTypeSelector<T*> // function pointer.
		{
			static_assert( DKTypeTraits<T*>::IsFunctionPointer, "Type is not function!");

			using FunctionTraits = DKFunctionPointerTraits<T*>;
			using ParameterTypeList = typename FunctionTraits::ParameterTypeList;
			template <typename... Ts> using _Invoker = FunctionObjectInvoker<T*, typename FunctionTraits::ReturnType, Ts...>;
			using Invoker = typename ParameterTypeList::template TypesInto<_Invoker>;
			using Signature = typename Invoker::Signature;
		};
		template <typename T, typename Func> struct MemberFunctionTypeSelector // member function.
		{
			static_assert( DKTypeTraits<Func>::IsMemberFunctionPointer, "Type is not member function!");

			using FunctionTraits = DKMemberFunctionPointerTraits<Func>;
			using ParameterTypeList = typename FunctionTraits::ParameterTypeList;
			template <typename... Ts> using _Invoker = FunctionMemberObjectInvoker<T, Func, typename FunctionTraits::ReturnType, Ts...>;
			using Invoker = typename ParameterTypeList::template TypesInto<_Invoker>;
			using Signature = typename Invoker::Signature;
		};

		////////////////////////////////////////////////////////////////////////////////
		// determine regular function or function object (functor),
		// not applicable to class member function.
		template <typename T> struct IdentifyFunction
		{
			template <typename U> static DKTrue HasFunctionOperator(decltype(&U::operator()));
			template <typename U> static DKFalse HasFunctionOperator(...);
			template <typename U, bool hasOp> struct _FunctionTraits;
			template <typename U> struct _FunctionTraits<U, true>
			{
				using Traits = typename DKTypeTraits<decltype(&U::operator())>::MemberFunctionTraits;
				enum {IsObjectConst = DKTypeTraits<U>::IsConst}; // test object is const or not.
				enum {IsFunctionConst = Traits::IsConst};        // test function is const or not.
				// if object is const, member function must be const.
				enum {IsCallable = IsObjectConst ? IsFunctionConst : 1};
			};
			template <typename U> struct _FunctionTraits<U, false>
			{
				using Traits = typename DKTypeTraits<U>::FunctionTraits;
				enum {IsCallable = Traits::IsFunctionPointer};
			};

			using Ref = typename DKTypeTraits<T>::ReferredType;
			using TypeTraits = DKTypeTraits<Ref>;
			enum {IsFunction = TypeTraits::IsFunction};
			enum {IsFunctionPointer = TypeTraits::IsFunctionPointer};
			enum {IsFunctionObject = decltype(HasFunctionOperator<Ref>(0))::Value};

			// if function is reference type, convert to pointer type.
			using Callable = DKCondType<IsFunction, Ref*, Ref>;
			using FunctionTraits = typename _FunctionTraits<Callable, IsFunctionObject>::Traits;
			enum {IsCallable = _FunctionTraits<Callable, IsFunctionObject>::IsCallable};
		};

		////////////////////////////////////////////////////////////////////////////////
		// determine class member function callable.
		template <typename T, typename Func> struct IdentifyMemberFunction
		{
			template <typename U> struct _Wrapper
			{
				static_assert( DKTypeTraits<U>::IsPointer == 0, "Type must not be a pointer");
				static_assert( DKTypeTraits<U>::IsReference == 0, "Type must not be a reference");

				_Wrapper(U& o) : object(o)									{}
				_Wrapper(U&& o) : object(std::forward<U>(o))				{}
				_Wrapper(const _Wrapper& w) : object(w.object)				{}
				_Wrapper(_Wrapper&& w) : object(std::forward<U>(w.object))	{}

				U& operator * ()	{return object;}

				_Wrapper& operator = (_Wrapper&&) = delete;
				_Wrapper& operator = (const _Wrapper&) = delete;

				U object;
			};

			template <typename U> struct _ObjectTraits
			{
				enum {IsPointer = DKTypeTraits<U>::IsPointer};
				using ObjectType = DKCondType<IsPointer, typename DKTypeTraits<U>::PointeeType, typename DKTypeTraits<U>::ReferredType>;
				enum {IsConst = DKTypeTraits<ObjectType>::IsConst};
				using Wrapper = DKCondType<IsPointer, ObjectType*, _Wrapper<ObjectType>>;
			};
			// special-case DKObject : DKObject regards as Wrapper.
			template <typename U> struct _ObjectTraits<DKObject<U>>
			{
				using ObjectType = U;
				enum {IsPointer = true};
				enum {IsConst = false};
				using Wrapper = DKObject<U>;
			};
			template <typename U> struct _ObjectTraits<DKObject<U>&>
			{
				using ObjectType = U;
				enum {IsPointer = true};
				enum {IsConst = false};
				using Wrapper = DKObject<U>;
			};
			template <typename U> struct _ObjectTraits<const DKObject<U>&>
			{
				using ObjectType = U;
				enum {IsPointer = true};
				enum {IsConst = true};
				using Wrapper = const DKObject<U>;
			};

			template <bool IsConst, typename U, typename F> struct _IsCallable
			{
				using FunctionTraits = DKMemberFunctionPointerTraits<F>;
				static_assert(FunctionTraits::IsFunctionPointer, "Type Func is not member function!");

				using ClassType = typename FunctionTraits::ClassType;
				using ObjectType = typename DKTypeTraits<U>::UnqualifiedReferredType;

				enum {IsObjectConst = IsConst};
				enum {IsFunctionConst = FunctionTraits::IsConst};

				// determine 'U' can call 'Func'.
				//enum {ConversionTest = DKTypeConversionTest<ObjectType*, ClassType*>()};	//error on VS2015
				enum {ConversionTest = Private::ConversionTest<ObjectType*, ClassType*>::Result };

				enum {ConstTest = IsObjectConst ? IsFunctionConst : 1};

				enum {Result = ConversionTest && ConstTest};
			};

			using FunctionTraits = typename DKTypeTraits<Func>::MemberFunctionTraits;
			using ObjectTraits = _ObjectTraits<T>;
			using PointerWrapper = typename ObjectTraits::Wrapper;

			enum {IsPointer = ObjectTraits::IsPointer};
			enum {IsConst = ObjectTraits::IsConst};
			enum {IsCallable = _IsCallable<ObjectTraits::IsConst, typename ObjectTraits::ObjectType, Func>::Result};
		};
	}

	/// Test whether given type T is a function or function-like object.
	template <typename T> using DKFunctionTest = Private::IdentifyFunction<T>;
	/// Retrieve function type traits (return-type, parameter type, etc.)
	template <typename T> using DKFunctionType = Private::FunctionTypeSelector<typename DKFunctionTest<T>::Callable>;

	/// Create DKFunctionSignature object from functor or function pointer
	template <typename Func> auto DKFunction(Func&& fn)-> DKObject<typename DKFunctionType<Func>::Signature>
	{
		using FunctionTest = DKFunctionTest<Func>;
		using FunctionTraits = typename FunctionTest::FunctionTraits;

		static_assert( FunctionTraits::IsFunctionPointer, "Type is not a function!");
		static_assert( FunctionTest::IsCallable, "Function is not callable");

		using FunctionType = DKFunctionType<Func>;
		using Invoker = typename FunctionType::Invoker;
		using Signature = typename FunctionType::Signature;

		return new(Private::FunctionAllocator<Invoker>::Instance()) Invoker(std::forward<Func>(fn));
	}

	template <typename T, typename Func> using DKFunctionMemberTest = Private::IdentifyMemberFunction<T, Func>;
	template <typename T, typename Func> using DKFunctionMemberType = Private::MemberFunctionTypeSelector<typename DKFunctionMemberTest<T, Func>::PointerWrapper, Func>;

	/// Create DKFunctionSignature object from class-member function (obj: object ptr or DKObject)
	template <typename T, typename Func> auto DKFunction(T&& obj, Func&& fn)-> DKObject<typename DKFunctionMemberType<T, Func>::Signature>
	{
		using FunctionTest = DKFunctionMemberTest<T, Func>;
		using FunctionTraits = typename FunctionTest::FunctionTraits;

		static_assert( DKTypeTraits<Func>::IsMemberPointer, "Type(Func) is not member pointer");
		static_assert( DKTypeTraits<Func>::IsMemberFunctionPointer, "Type(Func) is not member function!");
		static_assert( FunctionTest::IsCallable, "Member function is not callable!");

		using FunctionType = DKFunctionMemberType<T, Func>;
		using Invoker = typename FunctionType::Invoker;
		using Signature = typename FunctionType::Signature;

		return new(Private::FunctionAllocator<Invoker>::Instance()) Invoker(std::forward<T>(obj), fn);
	}
}

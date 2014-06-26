#pragma once

namespace yyy {

	//
	// choice
	//

	template <typename...Args> struct choice;

	//
	// either, maybe, nothing
	//

	template <typename,typename> struct either;

	template <typename A> struct either<A,void>;
	template <typename A> using maybe = either<A,void>;

	struct either<void,void>;
	using nothing = maybe<void>;

	//
	// type_set
	//

	using type_set = std::set<const std::type_info*>;

	//
	// either<void,void> / nothing
	//

	template <> struct either<void,void> {
		constexpr bool empty() const {
			return true;
		}
		void clear() const {
		}
		std::wstring str() const {
			return L"()";
		}
		template <typename> static constexpr bool allows_type() { 
			return false;
		}
		template <typename T,typename U> static constexpr bool allows_any_type(const either<T,U>&e) {
			return false;
		}
		template <typename> constexpr bool contains_type() const {
			return false;
		}
		template <typename T,typename U> constexpr bool contains_any_type(const either<T,U>& e) const {
			return false;
		}
		template <typename T> constexpr bool contains_value(const T&) const {
			return false;
		}
		template <typename T,typename U> constexpr bool contains_any_value(const either<T,U>& e) const {
			return false;
		}
		type_set get_types() const {
			return type_set();
		}
		static type_set allowed_types() {
			return type_set();
		}
	};

	//
	// either_helper, either_generic
	//

	namespace {
		template <typename A,typename T> struct either_helper {

			using either_type = either<A,void>;
			using value_type = A;
			using test_type = T;

			static const bool allows_type = false;
			static bool contains_type(const either_type&) {
				return false;
			}
			static bool contains_value(const either_type&, const test_type&) {
				return false;
			}
		};

		template <typename A> struct either_helper<A,A> {

			using either_type = either<A,void>;
			using value_type = A;
			using test_type = A;

			static const bool allows_type = true;
			static bool contains_type(const either_type& x) {
				return x.a_ptr not_eq nullptr;
			}
			static bool contains_value(const either_type& x, const test_type& a) {
				return contains_type(x) and ( a == *x.a_ptr );
			}
		};

		namespace either_generic {

			template <template <class> class K, typename T> static std::wstring str(const K<T>&t) {
				return t.str();
			}

			template <typename T> static std::wstring str(const T&t) {
				std::wstringstream ss;
				ss << t;
				return ss.str();
			}

			template <symbol_type S> static std::wstring str(const basic_symbol<wchar_t,S>& s) {
				return std::wstring(s);
			}

			template <typename T> static std::wstring str(const std::basic_string<T>& t) {
				return std::wstring(t.begin(),t.end());
			}
		}

	}

	//
	// either<A,void> / maybe<A>
	//

	template <typename A> struct either<A,void> {

		using a_type = A;

		a_type *a_ptr;

		//
		// constructors
		//

		constexpr either() : either(nullptr) {
		}
		either(const either& r) : either(r.a_ptr) {
		}
		either(const a_type& a) : either(&a) {
		}
		either(const a_type *my_a_ptr) : a_ptr(my_a_ptr ? new a_type(*my_a_ptr) : nullptr) {
		}

		//
		// insert, assignment
		//

		void assign(const a_type& a) {
			clear();
			a_ptr = new a_type(a);

		}

		void insert(const a_type& a) {
			assign(a);
		}

		template <typename T> void insert(const T&) {
			std::stringstream ss;
			ss << "insert of " << typeid(T).name() << " to " << typeid(*this).name() << " not possible.";
			throw std::runtime_error(ss.str());
		}

		template <typename T> void assign(const T& t) {
			insert(t);
		}

		//
		// empty, clear
		//

		constexpr bool empty() const {
			return a_ptr == nullptr;
		}

		void clear() {
			if(a_ptr) delete a_ptr;
			a_ptr = nullptr;
		}

		//
		// operators
		//

		either& operator=(const a_type& a) {
			assign(a);
			return *this;
		}

		either& operator=(const either& r) {
			clear();
			if(r.a_ptr) operator=(*r.a_ptr);
			return *this;
		}

		//
		// helper
		//

		template <typename T> using helper = either_helper<A,T>;

		//
		// string conversion
		//

		std::wstring str() const {
			return a_ptr ? either_generic::str(*a_ptr) : nothing().str();
		}

		//
		// destructor
		//

		~either() {
			clear();
		} 

		//
		// content and type checking
		//

		template <typename T> static constexpr bool allows_type() {
			return helper<T>::allows_type;
		}

		template <typename T,typename U> static constexpr bool allows_any_type(const either<T,U>&e) {
			return e.template contains_type<a_type>();
		}

		template <typename T> constexpr bool contains_type() const {
			return helper<T>::contains_type(*this);
		}

		template <typename T,typename U> constexpr bool contains_any_type(const either<T,U>& e) const {
			return ( a_ptr and e.template contains_type<a_type>() );
		}

		template <typename T> constexpr bool contains_value(const T& x) const {
			return helper<T>::contains_value(*this,x);
		}

		template <typename T,typename U> constexpr bool contains_any_value(const either<T,U>& e) const {
			return ( a_ptr and e.contains_value(*a_ptr) );
		}

		//
		// get the currently assigned types
		//

		static type_set allowed_types() {
			return type_set({&typeid(a_type)});
		}

		//
		// get the currently assigned types
		//

		type_set get_types() const {
			return a_ptr ? allowed_types() : type_set();
		}
	};
	
	//
	// recursive either<either,either>
	//

	template <typename A,typename B,typename C,typename D> struct either<either<A,B>,either<C,D>> {

		using a_type = either<A,B>;
		using b_type = either<C,D>;
		
		a_type *a_ptr;
		b_type *b_ptr;

		//
		// constructors
		//

		constexpr either() : either(nullptr,nullptr) {
		}
		either(const either& r) : either(r.a_ptr,r.b_ptr) {
		}
		either(const a_type *my_a_ptr, const b_type *my_b_ptr)
			: a_ptr(my_a_ptr ? new a_type(*my_a_ptr) : nullptr)
			, b_ptr(my_b_ptr ? new b_type(*my_b_ptr) : nullptr)
		{
		}

		//
		// empty, clear
		//

		constexpr bool empty() const {
			return ( a_ptr == nullptr or a_ptr->empty() )
			   and ( b_ptr == nullptr or b_ptr->empty() );
		}

		void clear() {
			if(a_ptr) delete a_ptr;
			if(b_ptr) delete b_ptr;
			a_ptr = nullptr;
			b_ptr = nullptr;
		}

		//
		// insert,assign
		//

		template <typename T> void insert(const T& t) {

			if( a_type::template allows_type<T>() ) {

				if(a_ptr == nullptr)
					a_ptr = new a_type();

				a_ptr->insert(t);

			} else if( b_type::template allows_type<T>() ) {

				if(b_ptr == nullptr)
					b_ptr = new b_type();

				b_ptr->insert(t);

			} else {

				std::stringstream ss;
				ss << "insert of " << typeid(T).name() << " to " << typeid(*this).name() << " not possible.";
				throw std::runtime_error(ss.str());
			}

		}

		template <typename T> void assign(const T& t) {
			clear();
			insert(t);
		}

		//
		// operators
		//

		either& operator=(const either& r) {
			clear();
			if(r.a_ptr) a_ptr = new a_type(*r.a_ptr);
			if(r.b_ptr) b_ptr = new b_type(*r.b_ptr);
			return *this;
		}

		template <typename T> either& operator=(const T& t) {
			assign(t);
			return *this;
		}

		//
		// string conversion
		//

		std::wstring str() const {

			     if(a_ptr and b_ptr) return a_ptr->str() + L',' + b_ptr->str();
			else if(a_ptr)           return a_ptr->str();
			else if(b_ptr)           return b_ptr->str();
			else                     return nothing().str();
		}
		
		//
		// wildcard constructor
		//

		template <typename T> either(const T& c) : either(nullptr,nullptr) {
			assign(c);
		}

		//
		// destructor
		//

		~either() {
			clear();
		}

		//
		// content and type checking
		//

		template <typename T> static constexpr bool allows_type() {
			return ( a_type::template allows_type<T>() )
			    or ( b_type::template allows_type<T>() );
		}

		template <typename T,typename U> static constexpr bool allows_any_type(const either<T,U>& e) {
			return ( a_type::allows_any_type(e) )
			    or ( b_type::allows_any_type(e) );
		}

		template <typename T> constexpr bool contains_type() const {
			return ( a_ptr and a_ptr->template contains_type<T>() )
			    or ( b_ptr and b_ptr->template contains_type<T>() );
		}

		template <typename T,typename U> constexpr bool contains_any_type(const either<T,U>& e) const {
			return ( a_ptr and a_ptr->contains_any_type(e) )
			    or ( b_ptr and b_ptr->contains_any_type(e) ); 
		}

		template <typename T> constexpr bool contains_value(const T& t) const {
			return ( a_ptr and a_ptr->contains_value(t) )
			    or ( b_ptr and b_ptr->contains_value(t) ); 
		}

		template <typename T,typename U> constexpr bool contains_any_value(const either<T,U>& e) const {
			return ( a_ptr and a_ptr->contains_any_value(e) )
			    or ( b_ptr and b_ptr->contains_any_value(e) ); 
		}

		//
		// get all of the allowed types
		//

		static type_set allowed_types() {

			type_set types;

			auto a_types( a_type::allowed_types() );
			auto b_types( b_type::allowed_types() );

			types.insert(a_types.begin(), a_types.end());
			types.insert(b_types.begin(), b_types.end());

			return types;
		}

		//
		// get the currently assigned types
		//

		type_set get_types() const {

			type_set types;

			auto a_types( a_ptr ? a_ptr->get_types() : type_set() );
			auto b_types( b_ptr ? b_ptr->get_types() : type_set() );

			types.insert(a_types.begin(), a_types.end());
			types.insert(b_types.begin(), b_types.end());

			return types;
		}
	};

	//
	// choice
	//

	template <> struct choice<> {
		using type = nothing;
		choice() = delete;
	};

	template <typename A> struct choice<A> {
		using type = maybe<A>;
		choice() = delete;
	};

	template <typename A, typename B> struct choice<either<A,B>> {
		using type = either<A,B>;
		choice() = delete;
	};

	template <typename A, typename...Args> struct choice<A,Args...> {
		using otherwise = typename choice<Args...>::type;
		using type = either<maybe<A>,otherwise>;
		choice() = delete;
	};

	template <typename A, typename B, typename...Args> struct choice<either<A,B>,Args...> {
		using otherwise = typename choice<Args...>::type;
		using type = either<either<A,B>,otherwise>;
		choice() = delete;
	};
}

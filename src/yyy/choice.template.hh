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
		template <typename> constexpr bool contains_type() const {
			return false;
		}
		template <typename> constexpr bool allows_type() const {
			return false;
		}
		template <typename T> constexpr bool contains_value(const T&) const {
			return false;
		}
		template <typename T,typename U> constexpr bool contains_any_value(const either<T,U>& e) const {
			return false;
		}
		template <typename T,typename U> constexpr bool contains_any_type(const either<T,U>& e) const {
			return false;
		}
		type_set get_types() const {
			return type_set();
		}
		type_set allowed_types() const {
			return type_set();
		}
	};

	//
	// either_helper
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

		constexpr either() : a_ptr(nullptr) {
		}

		either(const either& r) : a_ptr(r.a_ptr ? new a_type(*r.a_ptr) : nullptr) {
		}

		either(const a_type& a) : a_ptr(new a_type(a)) {
		}

		//
		// insert, assignment, empty, clear
		//

		void insert(const a_type& a) {
			if(a_ptr == nullptr)
				a_ptr = new a_type();
			*a_ptr = a;
		}

		template <typename T> void insert(const T&) {
			std::stringstream ss;
			ss << "insert of " << typeid(T).name() << " to " << typeid(*this).name() << " not possible.";
			throw std::runtime_error(ss.str());
		}

		template <typename T> void assign(const T& t) {
			clear();
			insert(t);
		}

		constexpr bool empty() const {
			return a_ptr == nullptr;
		}

		void clear() {
			if(a_ptr) {
				delete a_ptr;
				a_ptr = nullptr;
			}
		}

		//
		// string conversion
		//

		std::wstring str() const {
			if(a_ptr) {
				std::wstringstream ss;
				ss << *a_ptr;
				return ss.str();
			} else {
				return nothing().str();
			}
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

		template <typename T> using typing = either_helper<A,T>;

		template <typename T> constexpr bool allows_type() const {
			return typing<T>::allows_type;
		}
		template <typename T> constexpr bool contains_type() const {
			return typing<T>::contains_type(*this);
		}
		template <typename T> constexpr bool contains_value(const T& x) const {
			return typing<T>::contains_value(*this,x);
		}
		template <typename T,typename U> constexpr bool contains_any_value(const either<T,U>& e) const {
			return ( a_ptr and e.contains_value(*a_ptr) );
		}
		template <typename T,typename U> constexpr bool contains_any_type(const either<T,U>& e) const {
			return ( a_ptr and e.template contains_type<a_type>() );
		}

		//
		// get the currently assigned types
		//

		type_set allowed_types() const {
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

	namespace {
		struct either_generic {

			template <typename T> static constexpr bool empty(const T*const&ptr) {
				return ( not ptr ) or ptr->empty();
			}

			template <typename T> static void clear(T *&ptr) {
				if(ptr) {
					delete ptr;
					ptr = nullptr;
				}
			}

			template <typename T> static constexpr T *update(T*&l_ptr, const T*const&r_ptr) {
				clear(l_ptr);
				if(r_ptr)
					l_ptr = new T(*r_ptr);
				return l_ptr;
			}
		};
	}

	template <typename A,typename B,typename C,typename D> struct either<either<A,B>,either<C,D>> {

		using a_type = either<A,B>;
		using b_type = either<C,D>;
		
		a_type *a_ptr = nullptr;
		b_type *b_ptr = nullptr;

		//
		// constructors
		//

		either() {
		}

		either(const either& r) {
			either_generic::update(a_ptr,r.a_ptr);
			either_generic::update(b_ptr,r.b_ptr);
		}

		//
		// insert, assignment, empty, clear
		//


		constexpr bool empty() const {
			return either_generic::empty(a_ptr) and either_generic::empty(b_ptr);
		}

		void clear() {
			either_generic::clear(a_ptr);
			either_generic::clear(b_ptr);
		}

		template <typename T> void insert(const T& t) {

			if( a_type().template allows_type<T>() ) {

				if(a_ptr == nullptr)
					a_ptr = new a_type();

				a_ptr->insert(t);

			} else if( b_type().template allows_type<T>() ) {

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
		// string conversion
		//

		std::wstring str() const {

			     if(a_ptr and b_ptr) return a_ptr->str() + L',' + b_ptr->str();
			else if(a_ptr)           return a_ptr->str();
			else if(b_ptr)           return b_ptr->str();
			else                     return nothing().str();
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

		template <typename T> constexpr bool allows_type() const {
			return ( a_type().template allows_type<T>() )
			    or ( b_type().template allows_type<T>() );
		}
		template <typename T> constexpr bool contains_type() const {
			return ( a_ptr and a_ptr->template contains_type<T>() )
			    or ( b_ptr and b_ptr->template contains_type<T>() );
		}
		template <typename T> constexpr bool contains_value(const T& t) const {
			return ( a_ptr and a_ptr->contains_value(t) )
			    or ( b_ptr and b_ptr->contains_value(t) ); 
		}
		template <typename T,typename U> constexpr bool contains_any_value(const either<T,U>& e) const {
			return ( a_ptr and a_ptr->contains_any_value(e) )
			    or ( b_ptr and b_ptr->contains_any_value(e) ); 
		}
		template <typename T,typename U> constexpr bool contains_any_type(const either<T,U>& e) const {
			return ( a_ptr and a_ptr->contains_any_type(e) )
			    or ( b_ptr and b_ptr->contains_any_type(e) ); 
		}

		//
		// get all of the allowed types
		//

		type_set allowed_types() const {

			type_set types;

			auto a_types( a_type().allowed_types() );
			auto b_types( b_type().allowed_types() );

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

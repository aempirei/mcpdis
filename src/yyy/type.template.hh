#pragma once

// comment

#define template_recursive template <typename T,typename U,typename...Args>

#define template_recursive_with_types(...) template_recursive __VA_ARGS__(value_type& x)
#define template_recursive_with_values(...) template_recursive __VA_ARGS__(value_type& x, const T& t, const U& u, const Args &...args)

#define template_const_recursive_with_types(...) template_recursive __VA_ARGS__(const value_type& x)
#define template_const_recursive_with_values(...) template_recursive __VA_ARGS__(const value_type& x, const T& t, const U& u, const Args &...args)

#define template_functor_unary(_K_) template <template <typename...> class V, typename...Args> struct _K_<V<Args...>>
#define template_functor_binary(_K_) template <template <typename...> class V, typename...Args1, typename...Args2> struct _K_<V<Args1...>,V<Args2...>>

#define template_functor_unary_recursive_k0(_K_) template <template <typename...> class V> struct _K_<V<>>
#define template_functor_unary_recursive_k1(_K_) template <template <typename...> class V, typename T> struct _K_<V<T>>
#define template_functor_unary_recursive_kn(_K_) template <template <typename...> class V, typename T, typename...Args> struct _K_<V<T,Args...>>

namespace yyy {

	namespace type {

		template <typename T> std::type_index index() {
			return std::type_index(typeid(T));
		}

		template <typename...> struct list;
		template <typename...> struct container;

		template <typename,typename> struct equals;

		template <typename> struct _to_list;
		template <typename> struct _reverse;
		template <typename> struct _unique;

		template <typename,typename> struct _concat;
		template <typename,typename> struct _filter;
		template <typename,typename> struct _contains;

		template <typename T> using to_list = typename _to_list<T>::type;
		template <typename T> using reverse = typename _reverse<T>::type;
		template <typename T> using unique = typename _unique<T>::type;
		template <typename T, typename U> using concat = typename _concat<T,U>::type;
		template <typename T, typename U> using filter = typename _filter<T,U>::type;
		template <typename T, typename U> using contains = _contains<T,U>;

		using empty_list = list<>;

		// to_list<V<...>> -> list<...>
		//

		template_functor_unary(_to_list) {
			using type = list<Args...>;
			_to_list() = delete;
		};

		// concat<V<...>, V<...>> -> V<...>
		//

		template_functor_binary(_concat) {
			using type = V<Args1...,Args2...>;
			_concat() = delete;
		};

		// reverse<V<...>> -> V<...>
		//

		template_functor_unary_recursive_k0(_reverse) {
			using type = V<>;
			_reverse() = delete;
		};

		template_functor_unary_recursive_k1(_reverse) {
			using type = V<T>;
			_reverse() = delete;
		};

		template_functor_unary_recursive_kn(_reverse) {
			using type = concat<reverse<V<Args...>>, reverse<V<T>>>;
			_reverse() = delete;
		};

		// unique<V<...>> -> V<...>
		//

		template_functor_unary_recursive_k0(_unique) {
			using type = V<>;
			_unique() = delete;
		};

		template_functor_unary_recursive_k1(_unique) {
			using type = V<T>;
			_unique() = delete;
		};

		template_functor_unary_recursive_kn(_unique) {
			using filtered = filter<T,V<Args...>>;
			using tail = unique<filtered>;
			using type = concat<V<T>,tail>;
			_unique() = delete;
		};

		// _contains<T,V<...>> -> bool

		template <typename T, template <typename...> class V> struct _contains<T,V<>> {
			static constexpr bool eval = false;
			_contains() = delete;
		};

		template <typename T, template <typename...> class V, typename...Args> struct _contains<T,V<T,Args...>> {
			static constexpr bool eval = true;
			_contains() = delete;
		};

		template <typename T, template <typename...> class V, typename U,typename...Args> struct _contains<T,V<U,Args...>> {
			static constexpr bool eval = _contains<T,V<Args...>>::eval;
			_contains() = delete;
		};

		// filter<T,V<...>> -> V<...>

		template <typename T, template <typename...> class V> struct _filter<T,V<>> {
			using type = V<>;
			_filter() = delete;
		};

		template <typename T, template <typename...> class V, typename...Args> struct _filter<T,V<T,Args...>> {
			using type = filter<T,V<Args...>>;
			_filter() = delete;
		};

		template <typename T, template <typename...> class V, typename U,typename...Args> struct _filter<T,V<U,Args...>> {
			using tail = filter<T,V<Args...>>;
			using type = concat<V<U>,tail>;
			_filter() = delete;
		};

		// list<...>
		//

		template <> struct list<> {

			static constexpr int size = 0;
			static constexpr bool empty = true;

			template <template <typename...> class V> using bind = V<>;

			template <typename T> using push_front = list<T>;
			template <typename T> using push_back = list<T>;

			list() = delete;

			static const wchar_t *str() {
				return L"";
			};
		};

		template <typename T, typename...Args> struct list<T,Args...> {

			using head = T;
			using tail = list<Args...>;

			static constexpr int size = tail::size + 1;
			static constexpr bool empty = false;

			template <template <typename...> class V> using bind = V<T,Args...>;
			template <typename U> using push_front = list<U,Args...>;
			template <typename U> using push_back = list<Args...,U>;

			list() = delete;

			static std::wstring str() {
				std::wstringstream ss;
				ss << typeoperator[index<T>()] << L' ' << tail::str();
				std::wstring s = ss.str();
				if(not s.empty())
					s[s.length() - 1] = L'\0';
				return s;
			};
		};

		// container<...>
		//

		template <> struct container<> {

			using to_list = list<>;

			static constexpr int dim = to_list::size;

			constexpr int size() const {
				return 0;
			}

			template <typename U> bool contains() const {
				return false;
			}

			template <typename U> bool contains(const U&) const {
				return false;
			}

			template <typename U> constexpr U *find() const {
				return nullptr;
			}

			template <typename U> constexpr U **find_ref() const {
				return nullptr;
			}

			const wchar_t *str() const {
				return L"";
			};

			void clear() {
			}
		};

		template <typename T, typename...Args> struct container<T,Args...> {

			using to_list = list<T,Args...>;

			static constexpr int dim = to_list::size;

			constexpr int size() const {
				return head ? tail.size() + 1 : tail.size();
			}

			T* head;
			container<Args...> tail;

			container() : head(nullptr) {
			}

			container(const T& t, const Args&...args) : head(new T(t)), tail(args...) {
			}

			~container() {
				if(head) {
					delete head;
					head = nullptr;
				}
			}

			void clear() {
				if(head) {
					delete head;
					head = nullptr;
				}
				tail.clear();
			}

			container& operator=(const container& r) {
				if(this != &r) {
					if(head)
						delete head;
					head = r.head ? new T(*r.head) : nullptr;
					tail = r.tail;
				}
				return *this;
			}

			template <typename U> U *find() const {
				return equals<T,U>::eval ? (U *)head : tail.find<U>();
			}

			template <typename U> U **find_ref() const {
				return equals<T,U>::eval ? (U **)&head : tail.find_ref<U>();
			}

			template <typename U> U& get() const {
				static_assert(type::contains<U,container>::eval, "container::get<> called with unexpected type");
				return *find<U>();
			}

			template <typename U> void set(const U& u) {
				static_assert(type::contains<U,container>::eval, "container::set<> called with unexpected type");
				*find_ref<U>() = new U(u);
			}

			template <typename U> bool contains() const {
				return find<U>();
			}

			template <typename U> bool contains(const U& u) const {
				return contains<U>() and get<U>() == u;
			}


			std::wstring str() const {

				std::wstringstream ss;

				ss << typeoperator[index<T>()];

				if(head)
					ss << L':' << *head;

				ss << L' ' << tail.str();

				std::wstring s = ss.str();

				if(not s.empty())
					s[s.length() - 1] = L'\0';

				return s;
			}
		};

		// equals<T,T> -> bool
		//

		template <typename T,typename U> struct equals {
			equals() = delete;
			static constexpr bool eval = false;
		};

		template <typename T> struct equals<T,T> {
			equals() = delete;
			static constexpr bool eval = true;
		};

		using value_type = std::unordered_map<std::type_index,void *>;
		
		namespace map { 

			using type = value_type;

			// get<...>
			//

			template <typename T> T& get(value_type& x) {
				return *(T*)x.at(index<T>());
			}

			template <typename T> const T& get(const value_type& x) {
				return *(const T*)x.at(index<T>());
			}

			template_const_recursive_with_types(std::tuple<T,U,Args...> get) {
				return std::tuple<T,U,Args...>(get<T>(x),get<U>(x),get<Args>(x)...);
			}
			// contains_types<...>
			//

			template <typename T> bool contains_types(const value_type& x) {
				return x.find(index<T>()) != x.end();
			}

			template_const_recursive_with_types(bool contains_types) {
				return contains_types<T>(x) and contains_types<U,Args...>(x);
			}

			// contains_values<...>(...)
			//

			template <typename T> bool contains_values(const value_type& x, const T& t) {
				return contains_types<T>(x) and ( get<T>(x) == t );
			}

			template_const_recursive_with_values(bool contains_values) {
				return contains_values(x,t) and contains_values(x,u,args...);
			}

			// unassign<...>
			//

			template <typename T> void unassign(value_type& x) {
				if(contains_types<T>(x)) {
					delete (T*)x.at(index<T>());
					x.erase(index<T>());
				}
			}

			template_recursive_with_types(void unassign) {
				unassign<T>(x);
				unassign<U,Args...>(x);
			}

			// assign<...>(...)
			//

			template <typename T> T& assign(value_type& x, const T& t) {
				unassign<T>(x);
				auto *t_ptr = new T(t);
				x[index<T>()] = (void *)t_ptr;
				return *t_ptr;
			}

			template_recursive_with_values(void assign) {
				assign(x,t);
				assign(x,u,args...);
			}

			//
			//

			template <typename T> bool contains_type(const value_type& x) {
				return contains_types<T>(x);
			}

			template <typename T> bool contains_value(const value_type& x, const T& t) {
				return contains_values(x,t);
			}
		}
	}
}

#undef template_recursive

#undef template_recursive_with_types
#undef template_recursive_with_values

#undef template_const_recursive_with_types
#undef template_const_recursive_with_values

#undef template_functor_unary
#undef template_functor_binary

#undef template_functor_unary_recursive_k0
#undef template_functor_unary_recursive_k1
#undef template_functor_unary_recursive_k2

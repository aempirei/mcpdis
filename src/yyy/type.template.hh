#pragma once

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

	template <typename T> T *smart_copy(T *t) {
		return t ? new T(*t) : nullptr;
	}

	namespace type {

		template <typename T> std::type_index index() {
			return std::type_index(typeid(T));
		}

		template <typename T> wchar_t op() {
			return typeoperator[index<T>()];
		}

		template <typename T> std::wstring color() {
#ifdef USE_COLOR

			static std::unordered_map<std::type_index,std::wstring> data;

			if(data.find(index<T>()) == data.end()) {

				std::wstringstream ss;

				const char *p = typeid(T).name();

				int sum = 0;
				int round = 0;

				while(*p != '\0')
					sum += *p++ * ++round;

				sum %= 15;
				sum += 1;

				ss << "\x1b[" << int(sum > 8) << ';' << int(31 + (sum % 8)) << 'm';

				data[index<T>()] =  ss.str();
			}

			return data[index<T>()];
#else
			return L"";
#endif				
		}

		template <typename T> std::wstring to_str() {
			return std::wstring() + color<T>() + op<T>() + ANSI_CLR;
		}

		template <typename T> struct value_to_str {
			using value_type = T;
			static std::wstring call(const value_type& x) {
				std::wstringstream ss;
				ss << x;
				return ss.str();
			}
		};

		template <template <typename> class V, typename T> struct value_to_str<V<T>> {
			using value_type = V<T>;
			static std::wstring call(const value_type& x) {
				return x.str();
			}
		};

		template <typename> struct is_container;
		template <typename,typename> struct equals;

		template <typename...> struct list;
		template <typename...> struct container;

		template <typename> struct _to_list;
		template <typename> struct _reverse;
		template <typename> struct _unique;

		template <typename,typename> struct _concat;
		template <typename,typename> struct _filter;
		template <typename,typename> struct contains;

		template <typename T> using to_list = typename _to_list<T>::type;
		template <typename T> using reverse = typename _reverse<T>::type;
		template <typename T> using unique = typename _unique<T>::type;

		template <typename T, typename U> using concat = typename _concat<T,U>::type;
		template <typename T, typename U> using filter = typename _filter<T,U>::type;

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

		// contains<T,V<...>> -> bool
		//

		template <template <typename...> class V, typename T> struct contains<T,V<>> {
			static constexpr bool eval = false;
			contains() = delete;
		};

		template <template <typename...> class V, typename T, typename...Args> struct contains<T,V<T,Args...>> {
			static constexpr bool eval = true;
			contains() = delete;
		};

		template <template <typename...> class V, typename T, typename U, typename...Args> struct contains<T,V<U,Args...>> {
			static constexpr bool eval = contains<T,V<Args...>>::eval;
			contains() = delete;
		};

		// filter<T,V<...>> -> V<...>
		//

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

		// list<>
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

		// list<...>
		//

		template <typename T, typename...Args> struct list<T,Args...> {

			using head = T;
			using tail = list<Args...>;

			static constexpr int size = sizeof...(Args) + 1;
			static constexpr bool empty = false;

			template <template <typename...> class V> using bind = V<T,Args...>;
			template <typename U> using push_front = list<U,T,Args...>;
			template <typename U> using push_back = list<T,Args...,U>;

			list() = delete;

			static std::wstring str() {
				return tail::empty ? to_str<T>() : ( to_str<T>() + L" " + tail::str() );
			}
		};

		// container<>
		//

		template <> struct container<> {

			using to_list = list<>;

			template <typename T> using prepend = container<T>;
			template <typename T> using  append = container<T>;

			static constexpr int dim = to_list::size;

			constexpr int size() const { return 0; }

			constexpr bool empty() const { return true; }

			template <typename U> constexpr bool contains(        ) const { return false; }
			template <typename U> constexpr bool contains(const U&) const { return false; }

			template <typename U> constexpr U  *find    () const { return nullptr; }
			template <typename U> constexpr U **find_ref() const { return nullptr; }

			template <typename...Xs> container<Xs...>& project(container<Xs...>& r) const { return r; }
			template <typename...Xs> container<Xs...>& overlay(container<Xs...>& r) const { return r; }

			constexpr const wchar_t *     str() const { return L""; }
			constexpr const wchar_t *type_str() const { return L""; }

			constexpr bool operator==(const container&) const { return true; }

			constexpr operator const wchar_t * () const { return L""; }

			void clear() { }
		};

		// container<...>
		//

		template <typename T, typename...Args> struct container<T,Args...> {

			using to_list = list<T,Args...>;

			template <typename U> using prepend = container<U,T,Args...>;
			template <typename U> using append = container<T,Args...,U>;

			using head_type = T;
			using tail_type = container<Args...>;

			static constexpr int dim = to_list::size;

			constexpr int size() const {
				return head ? tail.size() + 1 : tail.size();
			}

			constexpr bool empty() const {
				return size() == 0;
			}

			head_type *head;
			tail_type tail;

			container() : head(nullptr), tail() {
			}

			container(const T& t, const Args&...args) : head(new T(t)), tail(args...) {
			}

			container(const container& r) : head(smart_copy(r.head)), tail(r.tail) {
			}

			template <typename U> container(const U& u) : container() {
				set(u);
			}

			void erase() {
				if(head) {
					delete head;
					head = nullptr;
				}
			}

			void clear() {
				erase();
				tail.clear();
			}

			~container() {
				erase();
			}

			container& operator=(const container& r) {
				if(this != &r) {
					if(head)
						delete head;
					head = smart_copy(r.head);
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

			template <typename U> container& operator=(const U& u) {
				set(u);
				return *this;
			}

			template <typename U> bool contains() const {
				return find<U>();
			}

			template <typename U> bool contains(const U& u) const {
				auto ptr = find<U>();
				return ptr and *ptr == u;
			}

			template <typename U> void unset() {
				static_assert(type::contains<U,container>::eval, "container::set<> called with unexpected type");
				auto ref = find_ref<U>();
				if(ref and *ref) {
					delete *ref;
					*ref = nullptr;
				}
			}

			template <typename...Xs> container<Xs...>& project(container<Xs...>& r) const {
				auto ref = r.find_ref<T>();
				if(ref) {
					if(*ref)
						delete *ref;
					*ref = smart_copy(head);
				}
				return tail.project(r);
			}

			template <typename...Xs> container<Xs...>& overlay(container<Xs...>& r) const {
				auto ref = r.find_ref<T>();
				if(ref and head) {
					if(*ref)
						delete *ref;
					*ref = new T(*head);
				}
				return tail.overlay(r);
			}

			std::wstring str() const {

				std::wstringstream ss;

				if(head) {
					ss << to_str<T>() << ':' << value_to_str<T>::call(*head);
					if(not tail.empty())
						ss << ',';
				}

				if(not tail.empty())
					ss << tail.str();

				return ss.str();
			}

			std::wstring type_str() const {
				return head ? ( to_str<T>() + tail.type_str() ) : tail.type_str();
			}

			bool operator==(const container& r) const {
				return ( ( head and r.head and *head == *r.head ) or not ( head or r.head ) ) and ( tail == r.tail );
			}
		};

		// is_container<>
		//

		template <typename T> struct is_container {
			static constexpr bool value = false;
		};

		template <typename...Args> struct is_container<container<Args...>> {
			static constexpr bool value = true;
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

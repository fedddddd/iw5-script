// https://github.com/momo5502/open-iw5

#pragma once

namespace utils
{
	class memory final
	{
	public:
		class allocator final
		{
		public:
			~allocator();

			void clear();

			void free(void* data);

			void free(const void* data);

			void* allocate(size_t length);

			template <typename T>
			T* allocate()
			{
				return this->allocate_array<T>(1);
			}

			template <typename T>
			T* allocate_array(const size_t count = 1)
			{
				return static_cast<T*>(this->allocate(count * sizeof(T)));
			}

			bool empty() const;

			//char* duplicate_string(const std::string& string);

		private:
			std::mutex mutex_;
			std::vector<void*> pool_;
		};

		static void* allocate(size_t length);

		template <typename T>
		static inline T* allocate()
		{
			return allocate_array<T>(1);
		}

		template <typename T>
		static inline T* allocate_array(const size_t count = 1)
		{
			return static_cast<T*>(allocate(count * sizeof(T)));
		}

		//static char* duplicate_string(const std::string& string);

		static void free(void* data);
		static void free(const void* data);

		static bool is_set(const void* mem, char chr, size_t length);

		static allocator* get_allocator();

	private:
		static allocator mem_allocator_;
	};
}

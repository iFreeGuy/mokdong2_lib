#pragma once

namespace mokdong
{
	template<class TYPE>
	class singleton
	{
	private:
		static TYPE* instance_;

		singleton() = default;

	public:
		static TYPE& instance()
		{
			if (instance_ == nullptr)
			{
				if (instance_ == nullptr)
				{
					instance_ = new TYPE();
				}
			}

			return *instance_;
		}

		static void terminate()
		{
			if (instance_ != nullptr)
			{
				delete instance_;
				instance_ = nullptr;
			}
		}
	};

	template<class TYPE>
	TYPE* singleton<TYPE>::instance_ = nullptr;

}

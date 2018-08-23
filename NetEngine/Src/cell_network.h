#ifndef __CELL_NET_WORK_H__
#define __CELL_NET_WORK_H__


namespace knet
{

	class CellNetwork
	{
	private:
		CellNetwork();
		~CellNetwork();

	public:
		static CellNetwork& Instance()
		{
			static CellNetwork s_instance;
			return s_instance;
		}

		void Init();
		void Fini();

	};

}; // end of namespace knet


#endif // __CELL_NET_WORK_H__
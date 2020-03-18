/*
Project: IO File Chunks Library
Date: 2013/12/21
Version: 2.5.1.7
*/

#ifndef _IO_FILE_CHUNKS_H_
#define _IO_FILE_CHUNKS_H_
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <list>

#include "lib_linkers\ioflinker.h"
#define MAKE_ID(a,b,c,d) ((a)|((b)<<8)|((c)<<16)|((d)<<24)) //32-bit Little-Endian
#define BID_FORM MAKE_ID('F','O','R','M')
#define BID_BODY MAKE_ID('B','O','D','Y')
#define BID_CAT MAKE_ID('C','A','T',' ')

struct							iof_header;
struct							iof_chunk;
class							smart_chunk;
class							iof_positions;
class							iof_writer;
class							iof_reader;

extern inline unsigned __int32				MAKE_ID32(unsigned __int32 rawID);
extern inline unsigned __int64				MAKE_ID64(unsigned __int64 rawID);
extern inline bool										check_header(const iof_header* reading_material);
extern inline bool										checkfor_subChunks(const iof_chunk* reading_material);
extern inline unsigned __int32				count_subChunks(const iof_chunk* reading_material);
extern iof_positions*								find_subChunks(const iof_chunk* reading_material);
extern iof_positions*								find_subChunks(const iof_chunk* reading_material, unsigned __int32 levels_to_read);
extern inline iof_chunk*							copy_to_chunk(const char* data);
extern inline iof_chunk*							cast_to_chunk(char* data);
extern inline const iof_chunk*				get_subChunk(const iof_chunk* parent_chunk, unsigned __int64 sub_position);
extern std::list<const iof_chunk*>		get_subChunks(const iof_chunk* reading_material, unsigned __int32 levels_to_read);
extern std::list<const iof_chunk*>		get_subChunks(const iof_chunk* reading_material);
extern std::list<const iof_chunk*>		list_Chunks(const iof_chunk* reading_material, unsigned __int32 levels_to_read);
extern std::list<const iof_chunk*>		list_Chunks(const iof_chunk* reading_material);

												///I/O File Header Structure - provides the required structure for a valid header
struct						iof_header
{
	union
	{
		struct
		{
			unsigned __int64		padding;
			unsigned __int32		BASEID_Code;
			union
			{
				char					ASID_Type[4];
				unsigned __int32		ASID_Code;
			};
			unsigned __int64		Size;
		}header;
		char					header_c[24];
	};
	iof_header()
	{
		header.ASID_Code = 0;
		header.BASEID_Code = 0;
		header.Size = 0;
		header.padding = 0;
	}
	iof_header(const char* data)
	{
		for (int i = 0; i < sizeof(iof_header); ++i)
			header_c[i] = data[i];
	}
	iof_header(const iof_header& obj)
	{
		for (int i = 0; i < sizeof(iof_header); ++i)
			header_c[i] = obj.header_c[i];
	}
};
												///I/O File Chunk Structure - provides the required structure for a valid chunk
struct						iof_chunk : iof_header
{
	char*					data;

	iof_chunk(){}
	iof_chunk(const iof_chunk& obj)
	{
		unsigned __int64 iSize = obj.header.Size - sizeof(iof_header);
		data = new char[iSize];

		for (int i = 0; i < sizeof(iof_header); ++i)
		{
			header_c[i] = obj.header_c[i];
		}
		for (int i = 0; i < iSize; ++i)
		{
			data[i] = obj.data[i];
		}
	}
	iof_chunk(unsigned __int32 _BASEID_Code, unsigned __int32 _ASID_Code, unsigned __int64 _Size_of_data, const char* _data)
	{
		header.BASEID_Code = _BASEID_Code;
		header.ASID_Code = _ASID_Code;
		header.Size = _Size_of_data + sizeof(iof_header);
		data = new char[_Size_of_data];
		for (int i = 0; i < _Size_of_data; ++i)
		{
			data[i] = _data[i];
		}
	}
	iof_chunk(unsigned __int32 _BASEID_Code, unsigned __int32 _ASID_Code, const iof_chunk* child_chunk)
	{
		header.BASEID_Code = _BASEID_Code;
		header.ASID_Code = _ASID_Code;
		int HeaderSize = sizeof(iof_header);
		header.Size = child_chunk->header.Size + HeaderSize;
		data = new char[child_chunk->header.Size];

		for (int i = 0; i < HeaderSize; ++i)
		{
			data[i] = child_chunk->header_c[i];
		}
		unsigned __int64 iSize = child_chunk->header.Size;
		std::cout << "Debug Data Size: " << iSize << std::endl;
		for (int i = HeaderSize; i < iSize; ++i)
		{
			data[i] = child_chunk->data[i - HeaderSize];
		}
	}
	~iof_chunk()
	{
		if (data != nullptr)
		{
			delete[] data;
		}
		data = nullptr;
	}
	const char* cast_chunk_to_array()const
	{
		char* R_array;
		R_array = new char[header.Size];
		for (int i = 0; i < sizeof(iof_header); ++i)
		{
			R_array[i] = header_c[i];
		}
		for (int i = sizeof(iof_header); i < header.Size; ++i)
		{
			R_array[i] = data[i - sizeof(iof_header)];
		}
		return R_array;
	}
	void debug_chunk()const
	{
		for (int i = 0; i < sizeof(iof_header); ++i)
		{
			std::cout << header_c[i];
		}
		for (int i = 0; i < header.Size - sizeof(iof_header); ++i)
		{
			std::cout << data[i];
		}
		std::cout << "<end>";
	}
};
												///I/O Smart Chunk - saves memory by having children chunks point to sub sections of their parent (in memory)
class						smart_chunk
{
	public:
		smart_chunk(){}
		smart_chunk(const smart_chunk& obj);
		smart_chunk(const iof_chunk* Parent);
		smart_chunk(iof_chunk& Parent);
		~smart_chunk();

		void										releaseData();
		void										load_Chunk(const iof_chunk* Parent);
		void										load_Chunk(iof_chunk& Parent);
		const iof_chunk&							operator[](int i);
		const iof_chunk*							copy_Parent()const;
		void										copy_Children(std::list<const iof_chunk*> &list_out);
		unsigned __int32							size()const;

	private:
		iof_chunk*									Parent;
		std::list<iof_chunk*>						Children;
};
												///I/O File Header Positions - provides a way of easily storing and accessing start positions of headers within an I/O File Chunk. It stores one Parent position for every Child Position.
class						iof_positions
{
	public:
		void												add_entries(unsigned __int64 Parent, unsigned __int64 Child);
		const unsigned __int32								count_entries();
		const unsigned __int64								get_parent_of(unsigned __int64 Child);
		std::list<const unsigned __int64>					get_children_of(unsigned __int64 Parent);
		std::list<const unsigned __int64>					get_all();

	private:
		enum { PARENT, CHILD };
		std::list<const unsigned __int64>					Positions[2];
		std::list<const unsigned __int64>::iterator			Parent_Itr;
		std::list<const unsigned __int64>::iterator			Child_Itr;
};
												///I/O File Writer - provides an easy way to write chunks of data to file
class						iof_writer
{
	public:
		~iof_writer();
		void 										save_File(wchar_t* File_Name);
		void 										save_File(std::string File_Name);
		void										push_back(const iof_chunk* data_entry);
		void										push_front(const iof_chunk* data_entry);
		void										release_Data();

	private:
		std::ofstream								FileHandle;
		std::list<const iof_chunk*>					Data_List;
};
												///I/O File Reader - provides an easy way to read chunks saved in a file
class						iof_reader
{
	public:
		~iof_reader();
		void 										open_File(wchar_t* File_Name);
		void 										open_File(std::string File_Name);
		void										close_File();

		const smart_chunk&							get_XBIG_Chunk();
		const smart_chunk&							operator[](int i);
		unsigned __int32							size();

	private:
		void										read_File();
		std::ifstream								FileHandle;
		std::list<smart_chunk*>						Data_List;
		smart_chunk*								XBIG_Chunk;
};


#endif
#ifndef TS_TABLES_H
#define TS_TABLES_H

#include "../jscpp.io/BaseIO.h"
#include <vector>

using namespace std;

class PAT
{
public:
	PAT();

	vector<U16>* getPrograms(InputStream*);
	
	~PAT();
};

class PMT
{
public:
	PMT();

	class Program_Info_Descriptor
	{
	public:
		Program_Info_Descriptor(U8 des_tag, U8 des_len, Buffer* des_data);

		U8 get_descriptor_tag();
		U8 get_descriptor_length();
		Buffer* get_descriptor_data();

		~Program_Info_Descriptor();

	private:
		U8 descriptor_tag;
		U8 descriptor_length;
		Buffer* descriptor_data;
	};

	class Component
	{
	public:
		Component(U8 stream_t, U16 element_pid, U8 es_info_len, Buffer* es_des);

		U8 get_stream_type();
		U16 get_elementary_pid();
		U8 get_es_info_length();
		Buffer* get_es_descriptors();

		~Component();

	private:
		U8 stream_type;
		U16 elementary_pid;
		U8 es_info_length;
		Buffer* es_descriptors;
	};

	vector<Program_Info_Descriptor*>* get_program_info_des(InputStream*, PAT*);
	vector<Component*>* get_components(InputStream*, PAT*);

	~PMT();
};

class CAT
{
public:
	CAT();

	class Descriptor
	{
	public:
		Descriptor(U8, U8, U16, U16);

		U8 get_descriptor_tag();
		U8 get_descriptor_length();
		U16 get_ca_system_id();
		U16 get_ca_pid();

		~Descriptor();

	private:
		U8 descriptor_tag;
		U8 descriptor_length;
		U16 ca_system_id;
		U16 ca_pid;
	};

	vector<Descriptor*>* get_desriptors(InputStream*);


	~CAT();
};

class Program
{
public:
	virtual U16 getProgramPid(void) = 0;
	virtual U16 getECMPid(void) = 0;
	virtual U16 getEMMPid(void) = 0;
	virtual ~Program();
};

class ProgramList
{
public:
	vector<Program*>* getProgramList(InputStream*, PMT*);
	~ProgramList();
};
#endif




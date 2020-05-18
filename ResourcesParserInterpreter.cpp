#include "ResourcesParserInterpreter.h"
#include <iostream>
#include <sstream>

#define ID(x) (x + 1)
#define MAKE_RESOURCE_ID(PACKAGE,TYPE,INDEX) ((PACKAGE<<24) | (TYPE<<16) | INDEX)

using namespace std;

const string ResourcesParserInterpreter::ALL_TYPE = "";
const string ResourcesParserInterpreter::ATTR_TYPE = "attr";
const string ResourcesParserInterpreter::DRAWABLE_TYPE = "drawable";
const string ResourcesParserInterpreter::MIPMAP_TYPE = "mipmap";
const string ResourcesParserInterpreter::LAYOUT_TYPE = "layout";
const string ResourcesParserInterpreter::ANIM_TYPE = "anim";
const string ResourcesParserInterpreter::ANIMATOR_TYPE = "animator";
const string ResourcesParserInterpreter::XML_TYPE = "xml";
const string ResourcesParserInterpreter::DIMEN_TYPE = "dimen";
const string ResourcesParserInterpreter::STRING_TYPE = "string";
const string ResourcesParserInterpreter::STYLE_TYPE = "style";
const string ResourcesParserInterpreter::COLOR_TYPE = "color";
const string ResourcesParserInterpreter::ID_TYPE = "id";
const string ResourcesParserInterpreter::INTEGER_TYPE = "integer";

void ResourcesParserInterpreter::parserResource(const string& type) {
	for(auto it : mParser->getResourceForPackageName()) {
		cout<<it.first<<endl;
		ResourcesParser::ResStringPoolPtr types = it.second->pTypes;
		for(uint32_t i = 0 ; i < types->header.stringCount ; i++) {
			string resType = ResourcesParser::getStringFromResStringPool(types, i);
			if(type==ALL_TYPE || type == resType){
				parserResource(it.second, ID(i), resType, "\t");
			}
		}
	}
}

void ResourcesParserInterpreter::parserResource(
		ResourcesParser::PackageResourcePtr packageRes,
		uint32_t typeId,
		const string& type,
		const string& tab) {
	for(ResourcesParser::ResTableTypePtr pResTableType : packageRes->resTablePtrs[typeId]) {
		bool showConfigDirectory = true;

		for(int i = 0 ; i < pResTableType->entries.size() ; i++) {
			if(pResTableType->entries[i] == nullptr){
				continue;
			}
			if(showConfigDirectory) {
				string config = getConfigDirectory(pResTableType->header.config, type);
				cout<<endl<<tab<<config<<endl;
				showConfigDirectory = false;
			}
			auto pEntry = pResTableType->entries[i];
			auto keys = packageRes->pKeys;
			parserEntry(
					MAKE_RESOURCE_ID(packageRes->header.id, typeId, i),
					keys,
					pEntry,
					pResTableType->values[i],
					type,
					tab + "\t");
		}
	}
}

void ResourcesParserInterpreter::parserEntry(
		uint32_t resId,
		ResourcesParser::ResStringPoolPtr pKeys,
		ResTable_entry* pEntry,
		Res_value* pValue,
		const string& type,
		const string& tab) {
	string key = ResourcesParser::getStringFromResStringPool(pKeys, pEntry->key.index);
	if(pEntry->flags & ResTable_entry::FLAG_COMPLEX) {
		cout <<tab<<key<<endl;
		ResTable_map_entry* pMapEntry = (ResTable_map_entry*)pEntry;
		ResTable_map* pMap = (ResTable_map*)pValue;
		if(pMapEntry->parent.ident!= 0) {
			cout <<tab <<"parent: " <<mParser->getNameForId(pMapEntry->parent.ident) <<endl;
		}

		for(int i = 0 ; i < pMapEntry->count ; i++) {
				cout
					<<tab
					<<mParser->stringOfValue(&(pMap+i)->value)
					<<endl;
		}
	}else{
		if(ID_TYPE != type) {
			string value = mParser->stringOfValue(pValue);
			cout <<tab <<key<<" = " <<value <<endl;
		}
	}
}

void ResourcesParserInterpreter::parserId(const string& id) {
	uint32_t uid;
	if(0 == strncmp(id.c_str(), "0x", 2)) {
		istringstream(id)>>hex>>uid;
	} else {
		istringstream(id)>>uid;
	}
	vector<ResourcesParser::ResTableTypePtr> resTableTypePtrs = mParser->getResTableTypesForId(uid);
	if(resTableTypePtrs.empty()) {
		cout <<"can't find resource for " <<id <<endl;
	} else {
		ResourcesParser::PackageResourcePtr pPackage = mParser->getPackageResouceForId(uid);
		uint32_t typeId = TYPE_ID(uid);
		uint32_t entryId = ENTRY_ID(uid);
		string type = ResourcesParser::getStringFromResStringPool(pPackage->pTypes, typeId-1);
		for(ResourcesParser::ResTableTypePtr pResTableType : resTableTypePtrs) {
			ResTable_entry* pEntry = pResTableType->entries[entryId];
			Res_value* pValue = pResTableType->values[entryId];
			if(nullptr != pEntry) {
				cout <<getConfigDirectory(pResTableType->header.config, type) << " : ";
				parserEntry(uid, pPackage->pKeys, pEntry, pValue, type, "");
				cout <<endl;
			}
		}
	}
}

std::string ResourcesParserInterpreter::findResource(
	ResourcesParser::PackageResourcePtr packageRes,
	uint32_t typeId,
	const string &type,
	const string &name,
	const string &subtype)
{
	string value = "";
	for (ResourcesParser::ResTableTypePtr pResTableType : packageRes->resTablePtrs[typeId])
	{
		for (int i = 0; i < pResTableType->entries.size(); i++)
		{
			if (pResTableType->entries[i] == nullptr)
				continue;
			
			auto pEntry = pResTableType->entries[i];
			auto keys = packageRes->pKeys;
			uint32_t resId = MAKE_RESOURCE_ID(packageRes->header.id, typeId, i);
			Res_value * pValue = pResTableType->values[i];
		
			string key = ResourcesParser::getStringFromResStringPool(keys, pEntry->key.index);
			if (key == name && subtype == pResTableType->header.config.toString())
			{
				if (pEntry->flags & ResTable_entry::FLAG_COMPLEX)
				{
					ResTable_map_entry *pMapEntry = (ResTable_map_entry *)pEntry;
					ResTable_map *pMap = (ResTable_map *)pValue;
					/*if (pMapEntry->parent.ident != 0)
						cout << "parent: " << mParser->getNameForId(pMapEntry->parent.ident) << endl;*/

					for (int i = 0; i < pMapEntry->count; i++)
					{
						value += mParser->stringOfValueRaw(&(pMap + i)->value);
						if (i < pMapEntry->count - 1)
							value += ",";
					}
				}
				else
				{
					if (ID_TYPE != type)
					{
						value = mParser->stringOfValueRaw(pValue);
					}
				}
				return value;
			}
		}
	}
	return value;
}

string ResourcesParserInterpreter::parserName(const string &name, const string &subtype, const string &default_value)
{
	for (auto it : mParser->getResourceForPackageName())
	{
		ResourcesParser::ResStringPoolPtr types = it.second->pTypes;
		for (uint32_t i = 0; i < types->header.stringCount; i++)
		{
			string resType = ResourcesParser::getStringFromResStringPool(types, i);
			string value = findResource(it.second, ID(i), resType, name, subtype);
			if (value != "")
				return value;
		}
	}
	return default_value;
}

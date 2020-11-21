#pragma once
#include <string>
#include <unordered_map>
#include <unordered_set>

struct sContent
{
	std::string Word;
	std::string Description;
};

class Dictionary
{
public:
	static std::unordered_set<std::string> ParseWord( const std::string& str );

	Dictionary();
	Dictionary( const std::string& filepath );

	std::unordered_map<std::string, std::string> GetContent() const;

	bool Load( const std::string& filepath );
	void SetTitle( const std::string& title );
	std::string GetTitle() const;
	std::string GetColour() const;
	void Insert( const sContent& content );
	void Remove( const std::string& word );
	bool Exist( const std::string& word );
	void Clear();
	size_t Size() const;

private:
	std::string mTitle;
	std::string mColour;

	std::unordered_map<std::string, std::string> mContent;
	static uint32_t mTotalDictionary;
};
#include "ScoreBoard.h"
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <sstream>
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

namespace
{
	const int FormatVersion = 1;
}

void ScoreBoard::Load(const std::string& path, std::string* message)
{
	mPath = path;
	mRecords.clear();
	mNextId = 1;
	mLastAddedId = -1;

	std::error_code ec;
	if (!std::filesystem::exists(std::filesystem::u8path(path), ec))
	{
		return;
	}

	std::ifstream file(std::filesystem::u8path(path), std::ios::binary);
	std::stringstream buffer;
	buffer << file.rdbuf();
	const std::string content = buffer.str();

	rapidjson::Document doc;
	doc.Parse(content.c_str());

	bool valid = !doc.HasParseError() && doc.IsObject()
		&& doc.HasMember("records") && doc["records"].IsArray();
	std::vector<ScoreRecord> loaded;
	if (valid)
	{
		for (const auto& item : doc["records"].GetArray())
		{
			if (!item.IsObject() || !item.HasMember("name") || !item["name"].IsString()
				|| !item.HasMember("chars") || !item["chars"].IsInt())
			{
				valid = false;
				break;
			}
			ScoreRecord record;
			record.id = (item.HasMember("id") && item["id"].IsInt()) ? item["id"].GetInt() : mNextId;
			record.name = item["name"].GetString();
			record.chars = item["chars"].GetInt();
			record.playedAt = (item.HasMember("playedAt") && item["playedAt"].IsInt64()) ? item["playedAt"].GetInt64() : 0;
			loaded.push_back(record);
			mNextId = std::max(mNextId, record.id + 1);
		}
	}

	if (valid)
	{
		mRecords = loaded;
		return;
	}

	// 壊れている。上書きして消さないよう退避してから空で始める
	mNextId = 1;
	const std::string backup = path + ".bak";
	std::filesystem::copy_file(std::filesystem::u8path(path), std::filesystem::u8path(backup),
		std::filesystem::copy_options::overwrite_existing, ec);
	if (message)
	{
		*message = "scores.json is broken; saved a copy as " + backup + " and started empty";
	}
}

int ScoreBoard::Add(const std::string& name, int chars, int64_t playedAt, bool* saved)
{
	ScoreRecord record;
	record.id = mNextId++;
	record.name = name;
	record.chars = chars;
	record.playedAt = playedAt;
	mRecords.push_back(record);
	mLastAddedId = record.id;

	const bool ok = Save();
	if (saved)
	{
		*saved = ok;
	}
	return record.id;
}

std::vector<ScoreRecord> ScoreBoard::GetSorted(size_t limit) const
{
	std::vector<ScoreRecord> sorted = mRecords;
	std::stable_sort(sorted.begin(), sorted.end(), [](const ScoreRecord& a, const ScoreRecord& b)
	{
		if (a.chars != b.chars)
		{
			return a.chars > b.chars;
		}
		if (a.playedAt != b.playedAt)
		{
			return a.playedAt < b.playedAt;
		}
		return a.id < b.id;
	});
	if (sorted.size() > limit)
	{
		sorted.resize(limit);
	}
	return sorted;
}

bool ScoreBoard::Save() const
{
	if (mPath.empty())
	{
		return false;
	}

	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	writer.StartObject();
	writer.Key("version");
	writer.Int(FormatVersion);
	writer.Key("records");
	writer.StartArray();
	for (const ScoreRecord& record : mRecords)
	{
		writer.StartObject();
		writer.Key("id");
		writer.Int(record.id);
		writer.Key("name");
		writer.String(record.name.c_str());
		writer.Key("chars");
		writer.Int(record.chars);
		writer.Key("playedAt");
		writer.Int64(record.playedAt);
		writer.EndObject();
	}
	writer.EndArray();
	writer.EndObject();

	// 一時ファイルに書き切ってから置き換える
	const std::string tmpPath = mPath + ".tmp";
	{
		std::ofstream out(std::filesystem::u8path(tmpPath), std::ios::binary | std::ios::trunc);
		if (!out)
		{
			return false;
		}
		out.write(buffer.GetString(), static_cast<std::streamsize>(buffer.GetSize()));
		out.flush();
		if (!out)
		{
			return false;
		}
	}

	std::error_code ec;
	std::filesystem::rename(std::filesystem::u8path(tmpPath), std::filesystem::u8path(mPath), ec);
	return !ec;
}

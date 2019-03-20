#pragma once
#include <stdlib.h>
#include <iostream>
#include <unordered_map>
#include <string>
#include <unordered_set>
#include <Windows.h>
#include <assert.h>
#include <fstream>
#include <algorithm>
#include <cppjieba/jieba.hpp>

class TextSimilarity
{
public:
	typedef std::unordered_map<std::string, int> wordFreq;//创建一个map里面存词语和对应的词频
	typedef std::unordered_set<std::string> wordSet;//创建了一个set来存放停用词
	TextSimilarity(std::string dict);//定义构造函数，参数为资源路径
	double getTextSimilarity(const char* file1, const char* file2);

//private:

	void getStopWordTable(const char* stopWordFile);//获取停用词，参数是停用词文件路径的字符串
	wordFreq getWordFreq(const char* file);//从文件中获取文本
	std::string UTF8ToGBK(std::string str);//转码
	std::string GBKToUTF8(std::string str);//转码
	//把已经统计词频的数组，按照value从大到小排序，
	std::vector<std::pair<std::string, int>> sortByValueReverse(wordFreq& wf);

	//给一个排序完成的vector，选取一定的单词，存放到set中，因为set有天然去重效果
	void selectAimWords(std::vector<std::pair<std::string, int>>& wfvec, wordSet& wset);
	//构建词频向量
	std::vector<double> getOneHot(wordSet& wset, wordFreq& wf);
	//计算余弦相似度
	double cosine(std::vector<double> oneHot1, std::vector<double> oneHot2);

	std::string DICT;
	std::string DICT_PATH;
	std::string HMM_PATH;
	std::string USER_DICT_PATH;
	std::string IDF_PATH;
	std::string STOP_WORD_PATH;
	cppjieba::Jieba _jieba;//定义jieba对象，用于分词

	wordSet _stopWordSet;//存放停用词
	int _maxWordNumber;//指定的取出单词的个数，比如在排序后的vector中取出前50个


};
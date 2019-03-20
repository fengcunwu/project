#include "TextSimilarity.h"

using namespace std;

TextSimilarity::TextSimilarity(string dict)//定义构造函数，参数为资源路径
	: DICT(dict)
	, DICT_PATH(dict + "/jieba.dict.utf8")
	, HMM_PATH(dict + "/hmm_model.utf8")	
	, USER_DICT_PATH(dict + "/user.dict.utf8")
	, IDF_PATH(dict + "/idf.utf8")
	, STOP_WORD_PATH(dict + "/stop_words.utf8")
	, _jieba(DICT_PATH,
		HMM_PATH,
		USER_DICT_PATH,
		IDF_PATH,
		STOP_WORD_PATH)
	, _maxWordNumber(20)
{
	getStopWordTable(STOP_WORD_PATH.c_str());//初始化停用词表
}

string TextSimilarity::GBKToUTF8(string s)
{
	int len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), -1, NULL, 0);
	WCHAR* str1 = new WCHAR[len];
	MultiByteToWideChar(CP_ACP, 0, s.c_str(), -1, str1, len);

	len = WideCharToMultiByte(CP_UTF8, 0, str1, -1, NULL, 0, NULL, NULL);
	char* str2 = new char[len];
	WideCharToMultiByte(CP_UTF8, 0, str1, -1, str2, len, NULL, NULL);
	string strOutUTF8(str2);
	delete[]str1;
	str1 = NULL;
	delete[]str2;
	str2 = NULL;
	return strOutUTF8;
}

string TextSimilarity::UTF8ToGBK(string s)
{
	int len = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, NULL, 0);
	WCHAR* str1 = new WCHAR[len];
	MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, str1, len);

	len = WideCharToMultiByte(CP_ACP, 0, str1, -1, NULL, 0, NULL, NULL);
	char* str2 = new char[len];
	WideCharToMultiByte(CP_ACP, 0, str1, -1, str2, len, NULL, NULL);
	string strOutGBK(str2);
	delete[]str1;
	str1 = NULL;
	delete[]str2;
	str2 = NULL;
	return strOutGBK;
}

TextSimilarity::wordFreq TextSimilarity::getWordFreq(const char* filename)//从文件中获取文本
{
	ifstream fin(filename);//打开文件
	if (!fin.is_open())//判断文件是否打开
	{
		cout << "open file " << filename << " fail" << endl;
		return wordFreq();//返回一个空的map
	}

	string line;//创建一个对象，来存读出每一行的字符串
	wordFreq wf;//创建一个map，来存放一个单词和其对应的词频
	while (!fin.eof())
	{
		//读出文本
		getline(fin, line);//在打开的文件中读出一行到line中
		//读进来的是GBK编码的，现在要转成UTF8
		line = GBKToUTF8(line);
		//开始分词
		vector<string> words;//创建一个数组，用来装分词之后的字符串
		_jieba.Cut(line, words, true);//分词
		//统计词频
		for (const auto& e : words)//在数组中取出单词，统计词频
		{
			//去掉停用词
			if (_stopWordSet.count(e) > 0)//判断改词是否在停用词表中，在的话就跳出
				continue;//不统计
			else
			{
				if (wf.count(e) > 0)//判断是否在map中，在的话就++ count大于0表示存在
					wf[e]++;
				else
					wf[e] = 1;//不在的话就赋值为1
			}
		}
	}
	fin.close();
	return wf;
}

void TextSimilarity::getStopWordTable(const char* stopWordFile)//获取停用词
{
	ifstream fin(stopWordFile);//从所给的路径下读出文件
	if (!fin.is_open())
	{
		cout << "open file" << stopWordFile << "fail" << endl;
		return;
	}

	string line;
	while (!fin.eof())
	{
		getline(fin, line);//将当前行的字符串获取到line中
		_stopWordSet.insert(line);
	}
	fin.close();
}

//排序函数
bool cmpReverse(pair<string, int> lp, pair<string, int> rp)
{
	return lp.second > rp.second;//按value的值逆序排列
}

//把已经统计词频的数组，按照value从大到小排序，
vector<pair<string, int>> TextSimilarity::sortByValueReverse(TextSimilarity::wordFreq& wf)
{
	vector<pair<string, int>> wfvector(wf.begin(), wf.end());
	sort(wfvector.begin(), wfvector.end(), cmpReverse);
	return wfvector;
}

//给一个排序完成的vector，选取一定的单词，存放到set中，因为set有天然去重效果
void TextSimilarity::selectAimWords(vector<pair<string, int>>& wfvec, TextSimilarity::wordSet& wset)
{
	int length = wfvec.size();
	int sz = length > _maxWordNumber ? _maxWordNumber : length;
	for (int i = 0; i < sz; i++)
	{
		wset.insert(wfvec[i].first);
	}
	//已经完成从两个文本取出来的词频编码
}

//构建词频向量
vector<double> TextSimilarity::getOneHot(TextSimilarity::wordSet& wset, TextSimilarity::wordFreq& wf)
{
	//遍历wset的每一个词，然后在wf这个map中查找词频，放入vector中返回
	vector<double> OneHot;
	for (const auto& e : wset)
	{
		if (wf.count(e))
			//OneHot存放value
			OneHot.push_back(wf[e]);
		else
			OneHot.push_back(0);
	}
	return OneHot;
}

//计算余弦相似度
double TextSimilarity::cosine(vector<double> oneHot1, vector<double> oneHot2)
{
	double modelOneHot1 = 0;
	double modelOneHot2 = 0;
	double dotProduct = 0;
	unsigned int i = 0;
	assert(oneHot1.size() == oneHot2.size());
	for (i = 0; i < oneHot1.size(); i++)
	{
		dotProduct += (oneHot1[i] * oneHot2[i]);
	}
	for (i = 0; i < oneHot1.size(); i++)
	{
		modelOneHot1 += pow(oneHot1[i], 2);
		modelOneHot2 += pow(oneHot2[i], 2);
	}
	modelOneHot1 = pow(modelOneHot1, 0.5);
	modelOneHot2 = pow(modelOneHot2, 0.5);

	return dotProduct / (modelOneHot1 * modelOneHot2);
}
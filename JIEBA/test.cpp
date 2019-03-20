#include "TextSimilarity.h"

using namespace std;

void testWordFreq()
{
	TextSimilarity ts1("dict");//创建一个对象，在当前目录下获取文件
	TextSimilarity ts2("dict");
	//创建一个map来装从这个文件路径下打开的文件，分词，统计词频后的结果
	TextSimilarity::wordFreq wf1 = ts1.getWordFreq("test1.txt");
	TextSimilarity::wordFreq wf2 = ts2.getWordFreq("test2.txt");
	//wf现在装的是分词，去停用词，统计词频的结果，打印出来
	/*for (const auto& w : wf)
	{
		cout << ts.UTF8ToGBK(w.first) << ": " << w.second << endl;
	}*/
	//对wf这个map里的数据按照value值进行逆序排序
	//创建一个数组来接收逆序排序之后的结果
	vector<pair<string, int>> wfvec1 = ts1.sortByValueReverse(wf1);
	vector<pair<string, int>> wfvec2 = ts2.sortByValueReverse(wf2);
	/*for (const auto& e : wfvec)
	{
		cout << ts.UTF8ToGBK(e.first) << ": " << e.second << endl;
	}*/
	TextSimilarity::wordSet wset;
	ts1.selectAimWords(wfvec1, wset);
	ts2.selectAimWords(wfvec2, wset);
	//现在set中存放的是选取的一定数量的单词
	for (const auto& e : wset)
	{
		cout << ts1.UTF8ToGBK(e) << ' ' ;
	}
	cout << endl;
	vector<double> oneHot1 = ts1.getOneHot(wset, wf1);
	vector<double> oneHot2 = ts2.getOneHot(wset, wf2);

    cout << "文件相似度为: " << ts1.cosine(oneHot1, oneHot2) << endl;

}

int main()
{
	testWordFreq();
	//cout << "niho" << endl;
	system("pause");
	return 0;
}
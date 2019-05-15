# project
  # 文本相似度项目总结
   - 主旨：首先需要明白的是这是基于词频的检测两个文本的相似度
   - 步骤：
     - 1，对文本的分词工作，我用的是jieba库，jieba库对文本进行分词，
     - 2，分词之后，这些词中含有大量的停用词，需要用jieba库中的停用词表来去停用词
     - 3，去掉停用词之后，对剩下的词，进行统计词频，并且把词语和词频构建成键值对，存放入unordered_map（查找速度快）中，
     - 4，将unordered_map中的内容，按照key的值逆序排序，并放入vector<pair<string, int>>中，
     - 5，选取一定数量的词语(前20个或者更多)作为比较对象，把两个文件的vector中的词放入unordered_set（有天然去重效果）中，至此构建了一个统一编码，unordered_set中的词是两个文本选取词语的并集，
     - 6，然后遍历同一个unordered_set，返回到unordered_map找到对应的词频，放入vector中，如果没有就置为0，
     - 7，两个文本会构成两个vector，这两个vector在一起求余弦相似度，得出两个文本是否相似，
     - 8，得出的结果越趋近于1就越是相似，越是趋近于0就越不相似。
# project
 # 聊天室项目总结
  - 主旨：首先需要知道的是这是一个基于CS框架的，具有客户端和服务端
 

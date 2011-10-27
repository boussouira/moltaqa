#ifndef CLHEADER_H
#define CLHEADER_H

#include <CLucene.h>
#include <CLucene/StdHeader.h>

#include <CLucene/_clucene-config.h>
#include <CLucene/config/repl_tchar.h>
#include <CLucene/config/repl_wchar.h>
#include <CLucene/util/CLStreams.h>
#include <CLucene/util/Misc.h>
#include <CLucene/util/StringBuffer.h>
#include <CLucene/util/dirent.h>
#include <CLucene/search/IndexSearcher.h>
#include <CLucene/search/QueryFilter.h>
#include <CLucene/queryParser/MultiFieldQueryParser.h>

#include <CLucene/highlighter/QueryTermExtractor.h>
#include <CLucene/highlighter/QueryScorer.h>
#include <CLucene/highlighter/Highlighter.h>
#include <CLucene/highlighter/TokenGroup.h>
#include <CLucene/highlighter/Formatter.h>
#include <CLucene/highlighter/SimpleFragmenter.h>
#include <CLucene/highlighter/SpanHighlightScorer.h>
#include <CLucene/analysis/CachingTokenFilter.h>

using namespace lucene::index;
using namespace lucene::analysis;
using namespace lucene::util;
using namespace lucene::store;
using namespace lucene::document;
using namespace lucene::queryParser;
using namespace lucene::search;
using namespace lucene::search::highlight;

#endif // CLHEADER_H

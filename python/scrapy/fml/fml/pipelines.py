# -*- coding: utf-8 -*-

# Define your item pipelines here
#
# Don't forget to add your pipeline to the ITEM_PIPELINES setting
# See: https://docs.scrapy.org/en/latest/topics/item-pipeline.html

import json

class FmlPipeline(object):
    def open_spider(self,spider):
        print("$"*120)
        self.fp = open("qsbk.json","w",encoding="utf-8")
        print("开始抓取爬虫文件")
        print("$"*120)

    def process_item(self, item, spider):
        item_json = json.dumps(dict(item),ensure_ascii=False)
        self.fp.write(item_json+'\n')
        return item
    
    def close_spider(self,spider):
        self.fp.close()
        print("$"*120)
        print("抓取爬虫文件结束")
        print("$"*120)
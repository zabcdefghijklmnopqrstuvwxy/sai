# -*- coding: utf-8 -*-

# Define your item pipelines here
#
# Don't forget to add your pipeline to the ITEM_PIPELINES setting
# See: https://docs.scrapy.org/en/latest/topics/item-pipeline.html

import json
from scrapy.exporters import JsonLinesItemExporter

class FmlPipeline(object):
    def __init__(self):
        self.fp = open("qsbk.json","wb")
        self.exporter = JsonLinesItemExporter(self.fp,ensure_ascii=False,encoding='utf-8')

    def open_spider(self,spider):
        print("$"*120)
        print("开始抓取爬虫文件")
        print("$"*120)

    def process_item(self, item, spider):
        self.exporter.export_item(item)
        return item
    
    def close_spider(self,spider):
        self.fp.close()
        print("$"*120)
        print("抓取爬虫文件结束")
        print("$"*120)
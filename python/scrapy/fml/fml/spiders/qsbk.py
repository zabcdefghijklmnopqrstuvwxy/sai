# -*- coding: utf-8 -*-
#scrapy创建一个工程命令 scrapy startproject [projectname]  
#scrapy创建一个爬虫文件 scrapy genspider [spidername] [spiderdomainname]
#scrapy开始抓取爬虫的命令 scrapy crawl [spidername]


import scrapy
from fml.items import FmlItem


class QsbkSpider(scrapy.Spider):
    name = 'qsbk'
    allowed_domains = ['qiushibaike.com']
    start_urls = ['https://www.qiushibaike.com/text/page/1/']
    base_domain = 'https://www.qiushibaike.com'
    
    def parse(self, response):
        storys = response.xpath("//div[@id='content-left']/div")
        print("#"*120)
        for story in storys:
            author = story.xpath(".//h2//text()").get().strip()
            content = story.xpath(".//div[@class='content']//text()").getall()
            content = "".join(content)
            item = FmlItem(author=author,content=content)
            yield item
        print("#"*120)

        next_url = response.xpath("//ul[@class='pagination']/li[last()]/a/@href").get()

        if not next_url:
            return
        else:
            yield scrapy.Request(self.base_domain+next_url,callback=self.parse)
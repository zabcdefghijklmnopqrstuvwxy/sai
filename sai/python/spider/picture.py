from urllib import request
import re


class SpiderImage(object):
	def __init__(self,url,rootpath):
		self.url = url
		self.rootpath = rootpath

	def __parse(self,url):
		reponse = request.urlopen(url)
		htmls = reponse .read()
		return htmls

	def __getimage(self,htmls):
		patten = r'http://[\S]*.jpg'
		img = re.findall(patten,repr(htmls))	
		num = 1			
		for imgdata in img:
			imgpath = self.rootpath + f'\{num}.jpg'
			with open(imgpath,"wb") as fp:
				imghtml = self.__parse(imgdata)
				fp.write(imghtml)
				print("正在下载第%s张图片"%num)
			num = num + 1

	def download(self):
		html = self.__parse(self.url)
		self.__getimage(html)
url = 'http://sc.chinaz.com/tupian/shuaigetupian.html'
rootpath = 'G:\Resource\picture\jpg'

img = SpiderImage(url,rootpath)
img.download()
				 
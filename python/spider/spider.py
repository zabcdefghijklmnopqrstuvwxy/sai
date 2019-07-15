
from urllib import request
import re

class Spider(object):
    url = "https://www.huya.com/g/2336"
    rootpatten = '<span class="txt">([\s\S]*?)</li>'
    prenamepatten = '<i class="nick" title=([\s\S]*?)</i>'
    namepatten = '"([\s\S]*?)"'
    numberpatten = '<i class="js-num">([\s\S]*?)</i>'

    def __fetch_content(self):
        r = request.urlopen(self.url)
        htmls = r.read()
        htmls = str(htmls,encoding='utf-8')
        #print(htmls)
        return htmls

    def __analysis(self,htmls):
        roothtml = re.findall(self.rootpatten,htmls)    
        anchors = []
        for html in roothtml:
            prename = re.findall(self.prenamepatten,html)
            name = re.findall(self.namepatten,str(prename))
            number = re.findall(self.numberpatten,html)
            anchor = {'name':name,'number':number}
            anchors.append(anchor)
        return anchors
    
    def __sortseed(self,anchor):
        r = re.findall("\d",str(anchor['number']))
        number = 0
        if '万' in str(anchor['number']): 
            for rank in range(0,len(r)):        
                number = number*10+float(r[rank])
        number = number*10000
        return number

    def __sort(self,anchors):
        anchors = sorted(anchors,key=self.__sortseed,reverse=True)
        return anchors

    def __show(self,anchors):
        for rank in range(0,len(anchors)):
            print('rank' + ':' + str(rank+1) + str(anchors[rank]['name']) + '------' + str(anchors[rank]['number']))

    def go(self):
        htmls = self.__fetch_content()
        anchors = self.__analysis(htmls)
        anchors = self.__sort(anchors)
        self.__show(anchors)

spider = Spider()
spider.go()

        





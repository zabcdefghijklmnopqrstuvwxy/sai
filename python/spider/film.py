import requests
import re

class Spider(object):
    headers = {'user_Agent':'Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/65.0.3325.181 Safari/537.36',
    'Referer':'https://movie.douban.com/' }
    def __init__(self,url):
        self.url = url

    def __gethtml(self):
        html = requests.get(self.url)
        filmdata = html.content.decode('utf-8')
        return filmdata

    def __parse(self,data):
        patten = '<li[\s\S]*?>'
        patname = 'data-title="([\s\S]*?)"'
        pattime = 'data-duration="([\s\S]*?)"'
        patgrade = 'data-score="([\s\S]*?)"'
        patpopular = 'data-votecount="([\s\S]*?)"'
        films = []
        filmdata = self.__gethtml()
        filterdata = re.findall(patten,filmdata)

        for data in filterdata:
            filmname = re.findall(patname,data)
            filmtime = re.findall(pattime,data)
            filmscore = re.findall(patgrade,data)
            filmpopular = re.findall(patpopular,data)
            film = {'name':filmname,'time':filmtime,'score':filmscore,'popular':filmpopular}
            if film['name']:
                films.append(film)
        return films   

    def __process(self):
        data = self.__gethtml()
        films = self.__parse(data) 
        films = sorted(films,key=lambda film:film['score'],reverse=True)
        return films

    def show(self):
        films = self.__process()
        for rank in range(0,len(films)):
            print("rank:"+str(rank+1)+'---'+str(films[rank]['name'])+'---'+str(films[rank]['time'])+'---'+str(films[rank]['score'])+'---'+str(films[rank]['popular']))

doubanurl = 'https://movie.douban.com/cinema/nowplaying/shenzhen/'
douban = Spider(doubanurl)
douban.show()




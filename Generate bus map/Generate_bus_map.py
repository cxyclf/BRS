from calendar import c
from itertools import count
from tempfile import tempdir
from tkinter import SE
import scipy.spatial as spt
import numpy as np
import math
from queue import PriorityQueue


cityName="Chengdu"
kValue=10 #k of knn
outFormat="{0}\tpoint\t1\t{1}\t666\tzdid\t{2}\t{3}\t{4}\t{5}\t{6}" #edgeid,start,stopname,lon,lat,length,end
cityRange={"lat_min":30.095,"lat_max":31.156,"lon_min":103.166,"lon_max":104.665}
neighborStopSize=4

def mydis(x1,y1,x2,y2):
    temp=(x1-x2)*(x1-x2)+(y1-y2)*(y1-y2)
    return math.sqrt(temp)
class VERTEX:
    def __init__(self):
        self.id=0
        self.lon=0
        self.lat=0
        self.outEdges=[]
        self.inRange=True
    def checkRange(self):
        if(self.lon < cityRange["lon_min"] or self.lon > cityRange["lon_max"]):
            self.inRange = False
        else:
            if(self.lat < cityRange["lat_min"] or self.lat > cityRange["lat_max"]):
                self.inRange = False

def clearDuplicate(stoppos, stoplist):
    hasStops=len(stoplist)
    stoppos=np.array(stoppos)
    index=np.argsort(stoppos)
    stoppos=stoppos[index]
    stoplist=np.array(stoplist)[index]
    duplicateList=[]
    temp=stoppos[-1]
    for i in range(hasStops-2,-1,-1):
         if(temp==stoppos[i]):
             duplicateList.append(i)
         else:
             temp=stoppos[i]
    for j in duplicateList:
        stoplist=np.delete(stoplist,j)
        stoppos=np.delete(stoppos,j)
    #hasStops -= len(duplicateList)
    return duplicateList,stoplist.tolist()

def test():
    pos=[1,3,2,5,4,2,2,3,5]
    slist=[1,2,3,4,5,6,7,8,9]
    print(clearDuplicate(pos,slist))

class STOP:
    def __init__(self,_id=-1, _name='', _lon=-1, _lat=-1):
        self.set(_id, _name, _lon, _lat)
        self.setedge(-1,-1)
        self.inRange=True
    def set(self, _id, _name, _lon, _lat):
        self.id=_id
        self.name=_name
        self.lon=_lon
        self.lat=_lat
    def setedge(self, _eid, _pos):
        self.eid=_eid
        self.pos=_pos
    def checkRange(self):
        if(self.lon < cityRange["lon_min"] or self.lon > cityRange["lon_max"]):
            self.inRange = False
        else:
            if(self.lat < cityRange["lat_min"] or self.lat > cityRange["lat_max"]):
                self.inRange = False
        
class EDGE:
    def __init__(self,_id=0,_start=0,_end=0,_length=0.0):
        self.set(_id,_start,_end,_length)
    def set(self,_id,_start,_end,_length):
        self.id=_id
        self.start=_start
        self.end=_end
        self.length=_length
        self.stoplist=[]
        self.inRange=True
                

class readData:
    def __init__(self, cityName):
        self.vertices=[]
        self.cityName = cityName
        self.iNumOfV = -1
        self.edges=[]
        self.vToE={}
    def readVertexCo(self):
        fin = open(self.cityName+".co")
        #skiy five lines
        for i in range(0,5):
            fin.readline()
        aLine=fin.readline()
        self.iNumOfV=int(aLine)
        self.vertices=[ VERTEX() for i in range(0, self.iNumOfV) ]
        #print(iNumOfV)
        for i in range(0, self.iNumOfV):
            aLine = fin.readline()
            aLine = aLine.split(' ')
            self.vertices[i].id=i
            self.vertices[i].lon=float(aLine[1])
            self.vertices[i].lat=float(aLine[0])
            self.vertices[i].checkRange()
            
        fin.close()
    def readRoad(self):
        fin = open(self.cityName+".road-d")
        #skiy five lines
        for i in range(0,5):
            fin.readline()
        eid=0
        while True:
            aLine = fin.readline()
            if not aLine:
                break
            aLine=aLine.split(' ')
            vstart = int(aLine[0])
            vend = int(aLine[1])
            if(vstart >= vend):
                continue
            self.vToE[(vstart, vend)]=eid
            length = float(aLine[2])
            tmpEdge=EDGE()
            tmpEdge.set(eid, vstart, vend, length)
            tmpEdge.inRange=(self.vertices[vstart].inRange and self.vertices[vend].inRange)
            self.edges.append(tmpEdge)
            self.vertices[vstart].outEdges.append(eid)
            self.vertices[vend].outEdges.append(eid)
            eid += 1     
    def readAll(self):
        self.readVertexCo()
        self.readRoad()
    def show(self):
        avgdegree=0
        count={}
        print("num of vertices:"+str(self.iNumOfV))
        print("num of edges:"+str(len(self.edges)))
        for i in range(0, self.iNumOfV):
            temp = len(self.vertices[i].outEdges)
            if temp in count:
                count[temp] += 1
            else:
                count[temp] = 1
            avgdegree += temp
        for i in count.keys():
            print("degree="+str(i)+":"+str(count[i]*100.0/self.iNumOfV)+"%")
        avgdegree = avgdegree*1.0/self.iNumOfV
        print("avgdeg:"+str(avgdegree))


class QianRu:
    def __init__(self, _cityName):
        self.cityName=_cityName
        self.data=readData(self.cityName)
        self.stops=[]
        self.newedges=[]
        self.newedgeNum=0
        self.existNewEdge={}
    def readStops(self):
        fin = open(self.cityName+".stops")
        tempid=0
        while True: 
            aLine = fin.readline()
            if not aLine:
                break
            aLine=aLine.split('\t')
            stopName=aLine[0]
            lon=float(aLine[1])
            lat=float(aLine[2])
            self.stops.append(STOP(tempid, stopName,lon,lat))
            self.stops[-1].checkRange()
            tempid += 1
            
    def makeIndex(self):
        self.pointsInRanges=[]
        for i in range(0, self.data.iNumOfV):
            if not self.data.vertices[i].inRange:
                continue
            self.pointsInRanges.append(i)
            
        self.points=np.array([[self.data.vertices[i].lon ,self.data.vertices[i].lat] for i in self.pointsInRanges ])
        self.kt = spt.KDTree(data=self.points)

    def locateAStop(self, iWhich):
        if not self.stops[iWhich].inRange:
            return
        #find its knn
        find_point=np.array([self.stops[iWhich].lon, self.stops[iWhich].lat])
        distance,position=self.kt.query(find_point, k=kValue)
        #change position to vid
        position=[self.pointsInRanges[i] for i in position]
        mindis,minc=math.inf,0
        minstart,minend=-1,-1
        for i in range(0, len(position)):
            for j in range(i+1, len(position)):
                vstart=position[i]
                vend=position[j]
                if(vstart > vend):
                    vstart, vend = vend, vstart
                if((vstart,vend) not in self.data.vToE):
                    continue
                dis,c=self.disToEdge(iWhich, vstart, vend)
                if(dis<mindis):
                    mindis,minc=dis,c
                    minstart,minend=vstart,vend
        if(minstart==-1):
            vstart=position[0]
            for j in self.data.vertices[vstart].outEdges:
                dis,c=self.disToEdge(iWhich, self.data.edges[j].start, self.data.edges[j].end)
                if(dis < mindis):
                    mindis,minc=dis,c
                    minstart,minend=self.data.edges[j].start, self.data.edges[j].end
                    
        eid=self.data.vToE[(minstart,minend)]

        self.data.edges[eid].stoplist.append(iWhich)
        self.stops[iWhich].setedge(eid, (1-minc)*self.data.edges[eid].length)
                
    def disToEdge(self, iWhich, vstart, vend):
        x0=self.stops[iWhich].lon
        y0=self.stops[iWhich].lat
        x1=self.data.vertices[vstart].lon
        y1=self.data.vertices[vstart].lat
        x2=self.data.vertices[vend].lon
        y2=self.data.vertices[vend].lat
        c=(x0-x2)*(x1-x2)+(y0-y2)*(y1-y2)
        d=(x1-x2)*(x1-x2)+(y1-y2)*(y1-y2)
        if(d==0):
            c=0
        else:
            c=c/d
        if(c<0):
            c=0
        if(c>1):
            c=1;
        x=c*(x1-x2)+(x2-x0)
        y=c*(y1-y2)+(y2-y0)
        dis=math.sqrt(x*x+y*y)
        return dis,c
    
    def printAnEdge(self,eid,fin):
        if(not self.data.edges[eid].inRange):
            return
        thisEdge=self.data.edges[eid]
        hasStops=len(thisEdge.stoplist)
        if(hasStops<3):
            return
        print(eid, thisEdge.start, thisEdge.end, thisEdge.length, hasStops, sep='\t',file=fin)
        if(hasStops > 0):
            for astop in thisEdge.stoplist:
                print(self.stops[astop].pos, end='\t',file=fin)
            print(file=fin)
            for astop in thisEdge.stoplist:
                print(self.stops[astop].name, end='\t',file=fin)
            print(file=fin)
    def writeNetwork(self):
        fout=open(self.cityName+'.map','w')
        for eid in range(0, len(self.data.edges)):
            self.printAnEdge(eid,fout)
    def showRange(self):
        xmin,xmax=self.points.min(0)
        ymin,ymax=self.points.max(0)
        xavg,yavg=np.average(self.points,axis=0)
        self.stopsCoordinator = np.array( [[self.stops[i].lon,self.stops[i].lat] for i in range(0,len(self.stops))] )
        sxmin,sxmax=self.stopsCoordinator.min(0)
        symin,symax=self.stopsCoordinator.max(0)
        sxavg,syavg=np.average(self.stopsCoordinator,axis=0)
        print(xmin,xmax,ymin,ymax,xavg,yavg)
        print(sxmin,sxmax,symin,symax,sxavg,syavg)
        
    def handleAStop(self, eid, sid, pq, visited):
        #pq must have store some vertices to be visited, visited must have store some vertices that not to be visited
        #print(eid,sid,end='  ')
        found=0
        while not pq.empty():
            if(found >= neighborStopSize):
                break
            current = pq.get() #current=(distance, id, is vertex or stop)
            if((current[1],current[2]) in visited):
                continue
            else:
                visited.add((current[1],current[2]))
            if(current[2] == 'stop'):
                #create a new edge
                leftid,rightid=sid,current[1]
                found += 1
                if(leftid>rightid):
                    leftid,rightid=rightid,leftid
                if((leftid,rightid) not in self.existNewEdge):
                    self.newedges.append(EDGE(self.newedgeNum, leftid,rightid, current[0]))
                    if(leftid==rightid):
                        print("error")
                    self.existNewEdge[(leftid,rightid)]=self.newedgeNum
                    self.newedgeNum += 1
                continue
            vid = current[1]         
            for ee in self.data.vertices[vid].outEdges:
                if ee==eid:
                    continue
                if not self.data.edges[ee].inRange:
                    continue
                start,end,edgeLength=self.data.edges[ee].start,self.data.edges[ee].end,self.data.edges[ee].length
                if(len(self.data.edges[ee].stoplist)):
                    #no need to expand from this edge, find the closest stop to vid.
                    #the closest stop may have inserted into pq because of another vertex, but no problem
                    closestStop, mylength = -1,-1
                    if(start==vid):
                        closestStop = self.data.edges[ee].stoplist[0]
                        mylength = self.stops[closestStop].pos
                    else:
                        closestStop = self.data.edges[ee].stoplist[-1]
                        mylength = edgeLength-self.stops[closestStop].pos
                    pq.put((current[0]+mylength, closestStop, 'stop'))
                    continue
                #to handle edge with no stops.
                if(start==vid):
                    anotherVertex=end
                else:
                    anotherVertex=start
                if((anotherVertex,'vertex') in visited):
                    continue
                pq.put((current[0]+edgeLength,anotherVertex,'vertex'))
    def handleStopsOnEdge(self, eid):
        hasStops=len(self.data.edges[eid].stoplist)
        if(hasStops==0):
            return
        start=self.data.edges[eid].start
        end=self.data.edges[eid].end
        length=self.data.edges[eid].length
        if(hasStops==1):
            sid = self.data.edges[eid].stoplist[0]
            visited=set()
            pos = self.stops[sid].pos
            pq=PriorityQueue()
            pq.put((pos, start,'vertex'))
            pq.put((length-pos, end,'vertex'))
            self.handleAStop(eid, sid, pq, visited)
        else:
            for i in range(0, hasStops-1):
                sid0=self.data.edges[eid].stoplist[i]
                sid1=self.data.edges[eid].stoplist[i+1]
                mylength=self.stops[sid1].pos-self.stops[sid0].pos
                #create edge
                if(sid0 > sid1):
                    sid0,sid1=sid1,sid0
                self.newedges.append(EDGE(self.newedgeNum, sid0, sid1, mylength))
                if(sid0==sid1):
                    print("error")
                self.newedgeNum += 1

            sidLeft=self.data.edges[eid].stoplist[0]
            visitedLeft=set([(end,'vertex')])
            pqLeft=PriorityQueue()
            pqLeft.put((self.stops[sidLeft].pos, start,'vertex'))
            self.handleAStop(eid, sidLeft, pqLeft, visitedLeft)
            
            sidRight=self.data.edges[eid].stoplist[-1]
            visitedRight=set([(start,'vertex')])
            pqRight=PriorityQueue()
            pqRight.put((length-self.stops[sidRight].pos, end,'vertex'))
            self.handleAStop(eid, sidRight, pqRight, visitedRight)
    def printANewEdge(self, eid, fout):
        thisedge=self.newedges[eid]
        start,end=thisedge.start,thisedge.end
        print(outFormat.format(1+eid,self.modifySID[start],self.stops[start].name,self.stops[start].lon,self.stops[start].lat,thisedge.length,self.modifySID[end]),file=fout)
    def writeStopsAsVertices(self):
        fout=open(self.cityName+'.xlsx','w')
        firstLine='\t\t\tpoint\troute\t\t\tlon\tlat\tlength\tconnection'
        secondLine='FID\tShape\tJoin_Count\tpoint\troute\tzdid\tmc\tlon\tlat\tlength\tconnection'
        print(firstLine, file=fout)
        print(secondLine, file=fout)
        #foreach stop, make it as a vertex, make an edge
        for eid in range(0, len(self.data.edges)):
            if not self.data.edges[eid].inRange:
                continue
            self.handleStopsOnEdge(eid)

                
        bcj,major=checkConnected(self.newedges)
        

        #modify the start and end because some duplicate stops are deleted, so the id is not consistence.
        self.modifySID={}
        countID=0
        for anEdge in self.newedges:
            start,end=anEdge.start,anEdge.end
            if(not foundParent(bcj, start)==major):
                continue
            if start not in self.modifySID:
                self.modifySID[start]=countID
                countID += 1
            if end not in self.modifySID:
                self.modifySID[end]=countID
                countID += 1
                
        for eid in range(0, len(self.newedges)):
            vstart=self.newedges[eid].start
            if(not foundParent(bcj, vstart)==major):
                continue
            self.printANewEdge(eid,fout)
        fout.close()        
        return
    def clearDuplicateStopOnEdge(self, eid):
        if(len(self.data.edges[eid].stoplist) > 1):
            stoppos=[ self.stops[iWhich].pos for iWhich in self.data.edges[eid].stoplist ]
            duplicateList,self.data.edges[eid].stoplist=clearDuplicate(stoppos, self.data.edges[eid].stoplist)
            for dup in duplicateList:
                self.stops[dup].setedge(-1, -1)
    def run(self):
        self.data.readAll()
        self.readStops()

        self.makeIndex()
        self.showRange()

        
        for iWhich in range(0,len(self.stops)):
            self.locateAStop(iWhich)
        for eid in range(0, len(self.data.edges)):
            self.clearDuplicateStopOnEdge(eid)
        self.writeNetwork()
        self.writeStopsAsVertices()
def foundParent(bcj, x):
    parent=x
    current=x
    while(parent>=0):
        current=parent
        parent=bcj[current]
    parent=x
    while(parent>=0):
        parent=bcj[x]
        if(x==current):
            break
        bcj[x]=current
        x=parent
    return current
def checkConnected(edges):
    bcj={}
    for anEdge in edges:
        start,end=anEdge.start,anEdge.end
        if start not in bcj:
            bcj[start]=-1
        if end not in bcj:
            bcj[end]=-1
        #merge
        p1=foundParent(bcj, start)
        p2=foundParent(bcj, end)
        if(p1==p2):
            continue
        num1=-bcj[p1]
        num2=-bcj[p2]
        if(num1<num2):
            bcj[p1]=p2
            bcj[p2]=-(num1+num2)
        else:
            bcj[p2]=p1
            bcj[p1]=-(num1+num2)
    count=0
    for value in bcj.values():
        if value < 0:
            print(-value)
            count += 1
    major=-1
    majorSize=0
    for key in bcj:
        value=-bcj[key]
        if (value > majorSize):
            majorSize= value
            major = key
    print("connected components:",count)
    return bcj,major
#test()
proj=QianRu(cityName)
proj.run()
#proj.writeStopsAsVertices()
#checkConnected(proj.newedges)
# a=np.array([[1,8],[3,4],[5,6]])
# x,y=a.min(0)
# print(x)
# print(y)
# print(a.min(0))
# print(a.max(0))
# print(a.max(1))
# print(np.average(a,axis=0))

        


logg = open('proc', 'r')


tudo = logg.readlines()
resp = {}
resp['kWayMerge'] = 0
resp['dump'] = 0
resp['refill'] = 0
resp['preprocess'] = 0
resp['cleanAll'] = 0

temp = {}
temp['kWayMerge'] = -1
temp['dump'] = -1
temp['refill'] = -1
temp['preprocess'] = -1
temp['cleanAll'] = -1

for line in tudo:
    line = line.split()
    if len(line) < 4: break
    while len(line[0]) < 19:
        line[0] += '0'

    if line[3] == '20906':
        if line[2] == 'start':
            temp[line[1]] = int(line[0])
        else:
            resp[line[1]] += (int(line[0]) - temp[line[1]]) 
    
print 'refill: ', (resp['refill'] * (10.0**-9))
print 'dump: ', (resp['dump'] * (10.0**-9))
print 'kWay: ', ((resp['kWayMerge'] * (10.0**-9)) - (resp['dump'] * (10.0**-9)) - (resp['refill'] * (10.0**-9)))
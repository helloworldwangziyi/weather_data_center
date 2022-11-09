###############################################
# 停止数据服务中心后台服务脚本
###############################################

killall -9 procctl
killall gzipfiles crtsurfdata deletefiles ftpgetfiles ftpputfiles tcpputfiles tcpgetfiles fileserver
killall obtcodetodb obtmindtodb execsql dminingmysql xmltodb
sleep 3

killall -9 gzipfiles crtsurfdata deletefiles ftpgetfiles ftpputfiles tcpgetfiles tcpputfiles fileserver
killall -9 obtcodetodb obtmindtodb execsql dminingmysql xmltodb
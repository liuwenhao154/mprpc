aux_source_directory(. SRC_LIST)
set(SRC_LIST 
    mprpcapplication.cc
    rpcprovider.cc
    rpcheader.pb.cc
    mprpcconfig.cc
    mprpcchannel.cc
    mprpccontroller.cc
    logger.cc
    zookeeperutil.cc)
add_library(mprpc ${SRC_LIST})
target_link_libraries(mprpc muduo_net muduo_base protobuf pthread zookeeper_mt)
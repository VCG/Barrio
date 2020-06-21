 new QWebChannel(qt.webChannelTransport, function(channel){
            // Set global Qt proxy object
            var jsobject = channel.objects.test;
            console.log(jsobject.myIntCppSide);           
        });


    // // Pass user name and password to QT
    // // Note: you can call this function on button click.
    // function toQT(name){
    //    qProxy.setData(name);
    // }
    var express = require ("express");
    var app = express();
    var body_parser= require('body-parser');
    app.use(body_parser.json());
    var database; 
    var MGresponse;
    //const x = require ("@googlemaps/google-maps-services-js");
    const {Client, Status} = require("@googlemaps/google-maps-services-js");
    const client = new Client({});
    var old_coord1="0.000,0.000";//"0.00,0.00";
    var old_coord2="0.000,0.000";
    var id1='0';
    var id2='0';
    

    // import { Status } from "../src/common";
    // import { decodePath } from "../src/util";
    //const directions = require("../src/directions");
    // const dir =  client.directions({
    //     params:{
    //         origin: "Mansiourieh,LB",
    //         destination: "Beirut,LB",
    //         waypoints: [],
    //         optimize: true,
    //         key: process.env.AIzaSyChJHd5Uj3VbwcrtLI25xsiaXOYRFCOHuc
    //     }
    // }).then((r) => r.json).then((data) => console.log((data.data.routes[0].legs[0].duration.text)));
    


    async function addAbribus(coordonne,ID,id,Dev_EUI){
        const collection = await database.collection("Abribus Identification");
        
    
        var obj={
            coord:coordonne,
            abri_ID:ID,
            Device_EUI:Dev_EUI,
            abribus_id:id,
    
        };
        collection.updateOne({coord: coordonne}, {$set: obj},{upsert:true},function(err, res) {
            if (err) throw err;
            console.log("Successfully updated the Bus stops collection");
          });
    
    
    }
    async function addBus(coordonne,ID,Dev_EUI,dest,next,allee,stops_list){
        const collection = await database.collection("Bus Identification");
        
    
        var obj={
            Device_EUI:Dev_EUI,
            Bus_ID:ID,
            Last_Destination:dest,
            next_stop:next,
            alle:allee,
            Bus_Stops:stops_list, };
        
        collection.updateOne({coord: coordonne}, {$set: obj},{upsert:true},function(err, res) {
            if (err) throw err;
            console.log("Successfully updated the Busses Collection");
          });
    }
    
    async function deleteBus(ID){
        const collection = await database.collection("Bus Identification");
        
        collection.deleteOne( { "Bus_ID" : ID } );
    
        
            console.log("Successfully deleted the Bus");
          
    }
    
    async function deleteAbribus(coordonne){
        const collectionBus = await database.collection("Bus Identification");
        const item = await collectionBus.find().toArray();
        const collection=await database.collection("Abribus Identification");
        
        collection.deleteOne( { "coord" : coordonne } );
    
        
        console.log("Successfully deleted the bus stops ");
        // for(it in item){
        // for(var i=0;i<item[it].Bus_Stops.length;i++){
        //     coord_stop=item[it].Bus_Stops[i];
        //     if(coord_stop==coordonne){

                collection.updateOne({},{$pull: {Bus_Stops: {$in:[coordonne]}}}, function(err, res){                  
        })

        }
//     }
// }}

    // Connection to MQTT Broker
    var mqtt = require('mqtt');

    // Client Connection
    //var clientMQTT = mqtt.connect('http://192.168.0.102:1883', {"username": "backend", "password": "pass"})
    //var clientMQTT = mqtt.connect('http://83.222.163.89:1883', {"username": "backend", "password": "pass"})

    var clientMQTT = mqtt.connect('http://18.191.57.118:1883', {"username": "backend", "password": "pass"})
    clientMQTT.on('connect', function () {
        console.log("Connected");
        // Client Subscription to Topic
        clientMQTT.subscribe('buses/bus1', function (err) {
            console.log("Subscribed");
            if(err){
                console.log("error");
            }
        })
        clientMQTT.subscribe('buses/bus2', function (err) {
            console.log("Subscribed");
            if(err){
                console.log("error");
            }
        })
    })

    // async function data_fetch(item){
    //     try{
    //     const collection = await database.collection("Bus Identification");
    //     const i_1=await c.findOne({Bus_ID:item});
    //     return i_1;
    // }catch(err){
    //     return err;
    // }
    // }

// async function calculate(time1,time2){
//     time11=time1.split(" ");
//     time21=time2.split(" ");
//     for(t in time11){
//         time12=t.split("");
//         if(time12[0])                        
//     }

// }



    async function data_fetch(message){
        console.log("---------------------I AM IN FETCH DATA-----------------------");
        const collection = await database.collection("Bus Identification");
        const item = await collection.find().toArray();
        const collectiond = await database.collection("Bus Driver Performance");
        var somme = 0;
        var stopsTime = [];
        for (i in item){

            if (item[i].Bus_ID==message.bus){
                            
                            var increment_next_stop=check_Next_Stop(message.coord,item[i].Bus_Stops[item[i].next_stop]);
                           
                            if(increment_next_stop==true){
                                console.log("checking next");
                                item[i].next_stop+=1;
                                Delete_from_Abribus(message.coord,item[i].next_stop,item[i].Bus_Stops);
                            }
                            
                                var stops=item[i].Bus_Stops;                                
                                var upcommingStops=stops.slice(item[i].next_stop,stops.length);
                            
                             
                                var upcommingStops=item[i].Bus_Stops.slice(item[i].next_stop,item[i].Bus_Stops.length);
                                console.log("upcomming",upcommingStops); 
                            
                            var busTimes = GetTime(message.coord,item[i].Bus_Stops[item[i].next_stop],upcommingStops,somme,stopsTime,item[i].next_stop,message.bus);
                            

                            
                            if(message.bus=='1'){
                                var coords = diviser(message.coord);
                                if(old_coord1!="0.000,0.000"){
                                var old_coords=diviser(old_coord1);
                                const dist = distance_on_geoid(old_coords[0],old_coords[1], coords[0], coords[1]);
                                const time_s = 15;
                                var speed_mps = dist / time_s;
                                var speed_kph = (speed_mps * 3600.0) / 1000.0;
                                
                                    if(speed_kph>0.1){
                                        collectiond.update({"bus_ID":message.coord}, {$set: {over_speed_limit: true}});
                                       
                                    }

                                console.log("vitesse:",speed_mps,"<-m/s  km/h->",speed_kph);}
                                old_coord1=message.coord;}
                            
                            if(message.bus=='2'){
                                var coords = diviser(message.coord);
                                if(old_coord2!="0.000,0.000"){
                                var old_coords=diviser(old_coord2);
                                const dist = distance_on_geoid(old_coords[0],old_coords[1], coords[0], coords[1]);
                                const time_s = 15;
                                var speed_mps = dist / time_s;
                                var speed_kph = (speed_mps * 3600.0) / 1000.0;
                                console.log("vitesse:",speed_mps,"<-m/s  km/h->",speed_kph);}
                                old_coord2=message.coord;}
                                if(speed_kph>0.1){
                                    collectiond.update({"bus_ID":message.bus}, {$set: {over_speed_limit: true}});
                                   
                                }
                            
                              
            } 
        }
    }

    async function Delete_from_Abribus(busID,nextstop,busStops){
        console.log("----------------I AM IN DELETE FROM ABRIBUS-------------------");
        const collection = await database.collection("Abribus Identification");
        const item = await collection.find().toArray();
        var id_bus="bus_"+(busID).toString();
        for (i in item){
            if(item[i].coord==busStops[nextstop-1]){
                collection.updateOne({coord: busStops[nextstop-1]}, {$set: {id_bus:"EMPTY"}});
            }
        }
    }

    function diviser(coordonne){
        var coord=coordonne.split(",");
        var lat=coord[0];
        var long=coord[1];
        console.log(coord);
        latnumb=parseFloat(lat);
        longnum=parseFloat(long);
        var a=[];


        a.push(lat);
        a.push(long);
        console.log(a);
        return a;

    }



async function Add_Time_To_Abribus(busTimes, abriBuses,nextStop,busID){
console.log("----------------I AM IN ADD TIME TO ABRIBUS-------------------");
 const collection = await database.collection("Abribus Identification");
 const item = await collection.find().toArray();
//AddTimeToAbribus(stopsTime, upcommingStops,nextStop,busID);
 for (q in item){
    busTimes_m=[];
    
    for(var i = 0; i<abriBuses.length; i++){

            if (item[q].coord==abriBuses[i]){
                console.log("value: ",busTimes[i]);
                var obj = {};
                var keyValue ={};
            
                
                keyValue[busID]=busTimes[i];
               
                obj["Bus_"+busID] = keyValue;
                collection.updateOne({coord: abriBuses[i]}, {$set: obj}, false, true);

                }

        }
    }
}

function distance_on_geoid( lat1, lon1, lat2, lon2) {
 
	// Convert degrees to radians
	lat1 = lat1 * Math.PI / 180.0;
	lon1 = lon1 * Math.PI / 180.0;
 
	lat2 = lat2 * Math.PI / 180.0;
	lon2 = lon2 * Math.PI / 180.0;
 
	// radius of earth in metres
	var r = 6378100;
 
	// P
	var rho1 = r * Math.cos(lat1);
	var z1 = r * Math.sin(lat1);
	var x1 = rho1 * Math.cos(lon1);
	var y1 = rho1 * Math.sin(lon1);
 
	// Q
	var rho2 = r * Math.cos(lat2);
	var z2 = r * Math.sin(lat2);
	var x2 = rho2 * Math.cos(lon2);
	var y2 = rho2 * Math.sin(lon2);
 
	// Dot product
	var dot = (x1 * x2 + y1 * y2 + z1 * z2);
	var cos_theta = dot / (r * r);
 
	var theta = Math.acos(cos_theta);
 
	// Distance in Metres
	return r * theta;
}

async function Publish_final_result(screen_id){
    console.log("-------------------I AM IN GET_FINAL_RESULT----------------------");
    const collection = await database.collection("Abribus Identification");
    const item = await collection.find().toArray();
    var result={};
    for (i in item){
        var name=screen_id.slice(17);
        
        if(item[i].abribus_id==name){
            result=item[i];
            console.log("tyyypeeeeeeeeeeeeeee",typeof(result));
            delete result.Device_EUI;
            delete result._id;
            delete result.coord;
            delete result.abri_ID;
            delete result.abribus_id;
            


// for(let [key,value] of Object.entries(result)){//2:213
    
//     if(key.includes("Bus_")){
//             console.log("c",c);
//             var bus="bus"+(c).toString();//bus2
//             var time="time"+(c).toString();//tim2
//             result[bus]= Object.keys(value)[0];//2
//             let x = Object.values(value)[0];
//             result[time] = (x/60).toString()+" min";
//         //{bus-1{..},,bus1:1,time1:153,bus2:1,time2:153}
//         c=c+1;
//     }
// }
    console.log("Test result --------- " ,result);




    clientMQTT.publish('abribuses/abribus1', JSON.stringify(result)); //to publish on the bus stop
           
        }
      
    }

}


function check_Next_Stop(message_coord,stop_coord){
    console.log(message_coord,stop_coord);
    console.log("------------------I AM IN CHECK NEXT STOP---------------------");
    var coord1=message_coord.split(",");
    var a=coord1[0].slice(0,7);
    var b=coord1[1].slice(0,7);
    var coord2=stop_coord.split(",");
    var c=coord2[0].slice(0,7);
    var d=coord2[1].slice(0,7);

    if( a==c&&b==d){
        return true;
    }
    else{
        return false;
    }
}


    // When someone else publishes data
    clientMQTT.on('message', function (topic, message) {
        // message is Buffer
        let message_str = JSON.parse(message.toString());
        console.log("message received from bus ", message_str);
       
        data_fetch(message_str);
        
       
        try{
        console.log("here");

        }catch(err){
            console.log(err)
        }
    }
    )


    // Get Data function to fetch it from MongoDB
    async function GetData (Collection){
    try{
        const collection = await database.collection(Collection);
        const item = await collection.find().toArray();
        return item;
    }catch(err){
        return err;
    }
    }


   

    // Client connection to MongoDbd
    // Main Route
    app.get('/', function(req, res){
    res.send("Hello from root - hii");
    });


    // Testing Route
    app.get('/test', function(req, res){
    try{
        GetData("test").then((data) => {
            res.send(data);
         })
    }catch(err){
        res.send(err)
    }
    });


    async function GetTime(depart ,arrive, upcommingStops, somme, stopsTime,nextStop,busID){//no bus??
        
        console.log("---------------I AM IN GET TIME----------------");
    client.directions({
        params:{
            /**
             * This is the DirectionsRequest. Note that origin, destination, travelMode and key are required fields. All the others are optional fields
             */
            origin:`${depart}`,//"33.8862,35.5088",//Long/lat format
            destination: arrive,//`Jbeil, Lebanon`,//String format
            travelMode: 'DRIVING',           
            key: "AIzaSyCKg_4LgTuwXxS2T4-RTOemM3R5KhUo7ks",
        }
    
    }).then((r) => {    
        if (r.data.status === Status.OK) {
            
        //console.log(r.data);
        //console.log(typeof(r.data.routes[0].legs[0].duration.value));
        console.log(r.data.routes[0].legs[0].duration.value,r.data.routes[0].legs[0].end_address);
        //console.log(upcommingStops.length);
        somme += r.data.routes[0].legs[0].duration.value;
        //console.log(stopsTime);
        stopsTime.push(somme);
        console.log("somme ",somme);
        console.log("stopsTime ", stopsTime);
        for(i = 0; i<upcommingStops.length-1; i++){
            console.log("time",i,i+1,upcommingStops);

            GetTime(upcommingStops[i], upcommingStops[i+1],[], somme, stopsTime,nextStop,busID);
            
            
            //send time to abribus later on or save its value here
        }
        //return r.data.routes[0].legs[0].duration.value;
        addAbribus("34.13887,35.71995",9,9,9);
        addBus(3,3,3,3,3,3,3,3);
        deleteBus(3);
        deleteAbribus("33.3333,35.5555");

        Add_Time_To_Abribus(stopsTime, upcommingStops,nextStop,busID);
        Publish_final_result('abribuses/abribus1');
        //Publish_final_result('abribuses/abribus2');

       

        return stopsTime;
            
    
        //return r.data.routes[0].legs[0].duration.text;
    } else {
        console.log(r.data.error_message);
        return [];
        
        // return undefined;
    }
    })
    .catch((e) => {
    console.log(e);
    });
    }
    



    // Get all items from Test
    app.get("/getAbribus", function(req, res){
    try{
        GetData("Abribus Identification").then((data) => {
            res.send(data);
        })
        
    }catch(err){
        res.send(err)
    }
    });

    // Get all the rows in Energy_Consumption Collection
    app.get("/getBus", function(req, res){
    try{
        GetData("Bus Identification").then((data) => {
            res.send(data);
            


            //    console.log(dir.data.routes[0].legs[0].duration.text);
        })
        
    }catch(err){
        res.send(err)
    }
    });

    // Get all items from Test
    app.get("/getBusDrivers", function(req, res){
    try{
        GetData("Bus Drivers' Performance").then((data) => {
            res.send(data);
            
        })
    }catch(err){
        res.send(err)
    }
    });


    const PORT = process.env.PORT || 3000;  

    app.listen(PORT, async () => {
        console.log("Listening on port: ", PORT);
        MongoClient = require("mongodb").MongoClient;
        DBConnectionString = 'mongodb+srv://admin:admin@cluster0-pw7m4.mongodb.net/test?retryWrites=true&w=majority';
        try{
            MGresponse = await MongoClient.connect(DBConnectionString, 
                {
                useNewUrlParser: true, 
                useUnifiedTopology: true 
            });
            database = await MGresponse.db("Abribus_Management");
            if(database){
                console.log("Database connected");
            }
            else{
                console.log("Connection error to database");
            }
        }catch(err){
            return err;
        }
    });
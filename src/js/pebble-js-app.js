var tmbHash = -1;

function updateTmbHash(callback){
	var req = new XMLHttpRequest();
	req.open('GET', 'https://pool.trademybit.com/api/hashinfo?key=TMB_API_KEY_HERE', true);
	req.onload = function(e){
		if (req.readyState == 4 && req.status == 200) {
			var response = JSON.parse(req.responseText);
			var rawHash = -1;
			try {
				rawHash = parseFloat(response.user_hash);
				
			} catch (err){
			}
			if (isNaN(rawHash) || rawHash < 0){
				tmbHash = -1;
			} else {
				tmbHash = Math.round(rawHash / 1000);
				console.log("Fetched hashrate data: " + tmbHash + " kH/s");
			}
		} else {
			tmbHash = -1;
		}
		callback(tmbHash);
	};
	req.send();
};


function spdCallback(spd){
        var transactionId = Pebble.sendAppMessage(
        {"0" : spd},
         function(e){
                 console.log("Successfully delivered message with transactionId="
						+ e.data.transactionId);
         },
         function(e){
                 console.log("Unable to deliver message with transactionId="
						+ e.data.transactionId
						+ " Error is: "
						+ e.error.message);
         });
}

function mainLoop(){
        updateTmbHash(spdCallback);
}

Pebble.addEventListener(
        "ready",
         function(e){
                 mainLoop();
                 setInterval(mainLoop, 60 * 1000);
         });
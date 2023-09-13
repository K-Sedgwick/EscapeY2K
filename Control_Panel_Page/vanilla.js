const onButton = document.getElementById('turn-led-on');
const offButton = document.getElementById('turn-led-off');

onButton.onclick = function(){
    const response = fetch("http://10.0.0.181:1234?led=on");
    console.log('onButton response', response);
}

offButton.onclick = function(){
    const response = fetch("http://10.0.0.181:1234?led=off");
    console.log('onButton response', response);
}

const onButton1 = document.getElementById('turn-led-on-1');
const offButton1 = document.getElementById('turn-led-off-1');

onButton1.onclick = function(){
    const response = fetch("http://10.0.0.225:1234?led=on");
    console.log('onButton response', response);
}

offButton1.onclick = function(){
    const response = fetch("http://10.0.0.225:1234?led=off");
    console.log('onButton response', response);
}
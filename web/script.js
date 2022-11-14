let baseUrl = "";
let myip;
let connectedStatus;

const isConnected = async () => {
    try {
        const response = await fetch(baseUrl);
        const myJson = await response.json();
        connectedStatus.innerText = myJson.message;
        connectedStatus.style.color = "green"
    } catch (error) {
        connectedStatus.innerText = "Not Connected";
        connectedStatus.style.color = "red"
        console.log(error);
    }
}
function changeIp() {
    baseUrl = `http://${myip.value}/`;
    isConnected();
}
const saveColor = async () => {
    const response = await fetch(`${baseUrl}savechanges`, { method: 'post' });
    const myJson = await response.json();
    console.log(myJson);
}
let isLoading = false;
async function watchColorPicker(event) {
    if (isLoading)
        return;
    const color = hexToRgb(event.target.value);
    try {
        isLoading = true;
        const response = await fetch(`${baseUrl}changecolor`, {
            method: 'post',
            body: JSON.stringify({
                "r": color.r,
                "g": color.g,
                "b": color.b
            })
        });
        const myJson = await response.json();
        // console.log(myJson);
    } catch (error) {
        console.log(error);
    }
    isLoading = false;
}
function hexToRgb(hex) {
    var result = /^#?([a-f\d]{2})([a-f\d]{2})([a-f\d]{2})$/i.exec(hex);
    return result ? {
        r: parseInt(result[1], 16),
        g: parseInt(result[2], 16),
        b: parseInt(result[3], 16)
    } : null;
}

function init() {
    const colorPicker = document.getElementById("colorpicker");
    myip = document.getElementById("myip");
    connectedStatus = document.getElementById("connected-status");
    baseUrl = `http://${myip.value}/`;
    colorPicker.addEventListener("input", watchColorPicker, false);
    colorPicker.addEventListener("change", watchColorPicker, false);
    isConnected();
    setInterval(isConnected, 5000);
}
init();
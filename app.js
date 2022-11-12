   //guardo como variables globales los items que quiero modificar mas adelante
 const defaultLegendClickHandler = Chart.defaults.plugins.legend.onClick;

let escala = document.getElementById("escala");

let temp = document.getElementById("temp");
let hum = document.getElementById("hum");
let bp = document.getElementById("bp");
let UltimaAct = document.getElementById("time");

let horas = document.getElementsByName("Thora");
let temperaturas = document.getElementsByName("Ttemp");
let humedades = document.getElementsByName("Thum");
let presiones = document.getElementsByName("Tbp");
let luz = document.getElementById("luz")

let upButton = document.getElementById("+");
let downButton = document.getElementById("-");

const socket = WSconnect();
const ctx = document.getElementById('myChart');
let myChart;

let bufferr

//-------------LOCAL STORAGE STUFF---------------
if(localStorage.getItem('vtemp') == undefined){
    console.log("llego aca!!");
    localStorage.setItem("vtemp", false);
    localStorage.setItem("vhum", true);
    localStorage.setItem("vbp", true);
    console.log(localStorage.getItem("vtemp"))
}

//----------FUNCIONES------------
const fecha = new Date()
function time(){
    let hora = fecha.getHours();
    hora = hora.toString();
    let minutos = fecha.getMinutes();
    minutos = minutos.toString();
    let mes = fecha.getMonth() + 1; 
    mes = mes.toString();
    let dia = fecha.getDate()
    dia = dia.toString();

    let actual = hora + ":" + minutos + " " + mes + "/" + dia;
    return actual
}

// Conexion al Web Socket
function WSconnect(){
    const socket = new WebSocket('wss://emetec.wetec.um.edu.ar/ws/simple');
    return socket
}
//funcion clickHandler (basicamente permite que las configuraciones del grafico se guarden)
const newLegendClickHandler = function(e, legendItem, legend) {
    const index = legendItem.datasetIndex;
    console.log(index);
    const ci = legend.chart;
    if (ci.isDatasetVisible(index)) {
        ci.hide(index);
        legendItem.hidden = true;
        if (index==1){localStorage.setItem("vhum", true);}
        else if(index==0){localStorage.setItem("vtemp", true);}
        else if(index==2){localStorage.setItem("vbp", true);}
    } else {
        ci.show(index);
        legendItem.hidden = false;
        if (index==1){localStorage.setItem("vhum", false);}
        else if(index==0){localStorage.setItem("vtemp", false);}
        else if(index==2){localStorage.setItem("vbp", false);}
    }
}

function grafico(x=grafica[0], temperatura=grafica[1], humedad=grafica[2], presion=grafica[3]){
    if (myChart) {myChart.destroy();}
    let y = parseInt(localStorage.getItem("escala"), 10);
    if(y>0){y+1;}
    x = x.slice(y, 50);
    temperatura = temperatura.slice(y, 50);
    humedad = humedad.slice(y, 50);
    presion = presion.slice(y, 50);
    escala.innerHTML = presion.length;

    myChart = new Chart(ctx, {
        type: 'line',
        data: {
            labels: x,
            datasets: [{
                label: 'Temperatura',
                data: temperatura,
                backgroundColor: [
                    'rgba(255, 99, 132, 0.2)',
                ],
                borderColor: [
                    'rgba(255, 99, 132, 1)',
                ],
                borderWidth: 1.5
            },
            {
                label: 'Humedad',
                data: humedad,
                backgroundColor: [
                    'rgba(54, 162, 235, 0.2)',
                ],
                borderColor: [
                    'rgba(54, 162, 235, 1)',
                ],
                borderWidth: 1.5        
            },
            {
                label: 'Presion Atmosferica',
                data: presion,
                backgroundColor: [
                    'rgba(255, 206, 86, 0.2)',
                ],
                borderColor: [
                    'rgba(255, 206, 86, 1)',
                ],
                borderWidth: 1.5        
            }
            
        ]
        },
        options: {
            plugins: {
                legend: {
                    display: true,
                    onClick: newLegendClickHandler
                }
            },
            animation: {
                duration: 1
            },
            scales: {
                y: {
                    beginAtZero: true
                }
            }
        }
    });
    function vhum(){
        if (localStorage.getItem("vhum")=="true"){
            return true
        }
        else{
            return false
        }
    }
    function vtemp(){
        if (localStorage.getItem("vtemp")=="true"){
            return true
        }
        else{
            return false
        }
    }
    function vbp(){
        if (localStorage.getItem("vbp")=="true"){
            return true
        }
        else{
            return false
        }
    }
    myChart.getDatasetMeta(0).hidden = vtemp();
    myChart.getDatasetMeta(1).hidden = vhum();
    myChart.getDatasetMeta(2).hidden = vbp();
    myChart.update();
}

function upScale(){
    let escala = parseInt(localStorage.getItem("escala"), 10);
    if (escala==0){return}
    else{
        escala--;
        localStorage.setItem("escala", escala.toString());
        if (escala==0){downButton.disabled = true}
        upButton.disabled = false;
        grafico(bufferr[0], bufferr[1], bufferr[2], bufferr[3]);
    }
}

function downScale(){
    let escala = parseInt(localStorage.getItem("escala"), 10);
    if (escala==40){return}
    else{
        escala++;
        localStorage.setItem("escala", escala.toString());
        if (escala==40){upButton.disabled = true}
        downButton.disabled = false
        grafico(bufferr[0], bufferr[1], bufferr[2], bufferr[3]);
    }
}
//-------KEYBOARD INPUT HANDLER---------
document.addEventListener('keydown', (event) => {
    var keyValue = event.key;

    if(keyValue.toString() == "+"){
        upScale();
    }
    else if(keyValue.toString() == "-"){
        downScale();
    }
  }, false);
//-----------EVENTOS WS------------
socket.addEventListener("error", function(event){
    console.log(event.data)
});


// evento de conexion al WS
socket.addEventListener('open', function (event) {
    console.log('Connected to WS Server')
    socket.send("hi")
});


// esto escucha mensajes del servidor
socket.addEventListener('message', function (event) {
    //console.log('Mensaje del servidor: ', event.data);
    let eventData = event.data.toString();
    if (!eventData.startsWith("{")){return}
    else{
        console.log(event.data);
        let data = JSON.parse(event.data);
        
        console.log(data["temp"]);
        let Dstr = Object.keys(data).length;
        let actual = time();
        UltimaAct.innerHTML = actual;
        //esto cambia los valores del documento segun lo que me mande el servidor
        temp.innerHTML = data["temp"];
        hum.innerHTML = data["hum"];
        bp.innerHTML = data["bp"];
        UltimaAct.innerHTML = ` ‎Hora: ${data["hora"][0]} ‎ ‎ | ‎ ‎ Fecha: ${data["hora"][1]}`;
        if (data["luz"] == "sol"){
            luz.src = "recursos/sol.png";
        } else if (data["luz"] == "luna") {
            luz.src = "recursos/Luna.png";
        } else if (data["luz"] == "nube"){
            luz.src = "recursos/nube.png"
        }else{
            luz.src = "";
        }

        let contador = 0;
        let largo = data["graph"][1].length-1;


        for (let i = 5; i>=0; i--){
            horas[contador].innerHTML = data["graph"][0][largo-i-1][0];
            temperaturas[contador].innerHTML = data["graph"][1][largo-i-1];
            humedades[contador].innerHTML = data["graph"][2][largo-i-1];
            presiones[contador].innerHTML = data["graph"][3][largo-i-1];
            contador = contador+1;

            if (i==0){
                grafico(data["graph"][0], data["graph"][1], data["graph"][2], data["graph"][3]);
                localStorage.setItem("grafica", JSON.stringify(data["graph"]));
                bufferr = data["graph"]
            }
        };

        //aca guardo en el almacenamiento local los datos por si ocurre algo con la conexion del WS
        localStorage.setItem("valoresSensores", JSON.stringify(data));
        
    }
});
if(localStorage.getItem("grafica")== undefined){
    let algo = [["no data", "no data", "no data"], [0, 0, 0], [0, 0, 0], [0, 0, 0]]
    localStorage.setItem("grafica", JSON.stringify(algo));
    localStorage.setItem("escala", 0)
}

let grafica = JSON.parse(localStorage.getItem("grafica"));

window.addEventListener("load", Init, false);
function Init(){
    let escala = parseInt(localStorage.getItem("escala"), 10);
    if (escala==40){upButton.disabled = true}
    if (escala==0){downButton.disabled = true}
}

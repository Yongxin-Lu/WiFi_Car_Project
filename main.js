var ms_send;
var Lasing_send;
var cam_X = 75;   //定义摄像头初始位置（回中)
var cam_Y = 110;
volt_service = setInterval(function () { ws.send("@"); }, 1000);

function Lasing(x) {  //激光发射指令发送
    switch (x) {
        case 1:
            Lasing_send = setInterval(function () { ws.send("*!1\r\n") }, 35);
            break;
        case 2:
            Lasing_send = setInterval(function () { ws.send("*!2\r\n") }, 35);
            break;
        case 3:
            clearInterval(Lasing_send);
            ws.send("*!3");
            break;

    }
}

function send_ord(order) {  //启动周期发送电机移动指令函数
    if (order != 0) {
        ms_send = setInterval(function () { send(order) }, 20);
    } else {
        clearInterval(ms_send);
        send(0);
    }
}

function send(order) {   //电机移动指令发送函数
    var speed = getSpeed();
    switch (order) {
        case 0:
            ws.send("#R000R000");
            break;
        case 1:
            ws.send("#F" + speed + "F" + speed);
            break;
        case 2:
            ws.send("#B" + speed + "B" + speed);
            break;
        case 3:
            ws.send("#B110F110");    //转向速度110
            break;
        case 4:
            ws.send("#F110B110");
            break;
    }
}

function cam_change(y, x, z) {
    switch (z) {  //启动周期发送摄像头移动指令函数
        case 1:
            ms_send = setInterval(function () { cam_move(y, x) }, 35);
            break;
        case 0:
            clearInterval(ms_send);
            break;
        case 2:
            ws.send("$110075\r\n");
            cam_X = 75;
            cam_Y = 110;
            document.getElementById("cam_pos").innerHTML = "cam x=75,y=110";
            break;
    }
}

function cam_move(y_arg, x_arg) {  //摄像头舵机移动指令发送函数
    var x = cam_X + x_arg;
    var y = cam_Y + y_arg;
    if (x < 0) {
        cam_X = 0;
    } else if (x > 180) {
        cam_X = 180;
    } else {
        cam_X = x;
    }
    x = ("00" + cam_X).substring(cam_X.toString().length - 1);
    if (y < 20) {
        cam_Y = 20;
    } else if (y > 180) {
        cam_Y = 180;
    } else {
        cam_Y = y;
    }
    y = ("0" + cam_Y).substring(cam_Y.toString().length - 2);
    ws.send("$" + y + x + "\r\n");
    document.getElementById("cam_pos").innerHTML = "cam x=" + cam_X + ",y=" + cam_Y;
}

function getSpeed() {  //获取滑动条数值作为“油门"
    var speed = document.getElementById("slider1").value;
    return ("0" + speed).substring(speed.length - 2);
}

var ws = new WebSocket("ws://192.168.2.138:8081/ws");  //与服务器建立Web Socket连接
ws.onmessage = function (evt) {
    if (evt.data == "OK") {
        document.getElementById("sta").innerHTML = "Connected!";
    } else if (evt.data.substring(0, 1) == "@") {
        volt_value = parseInt(evt.data.substring(1, 4));
        volt_value = (volt_value * 0.024).toFixed(2);
        document.getElementById("volt").innerHTML = volt_value + "V / " + (volt_value / 3).toFixed(2) + "V";
    }

}

window.onload = function () {           //事件监听
    window.addEventListener('touchstart', function (event) {   //禁止双击放大
        if (event.touches.length > 1) {
            event.preventDefault();
        }
    });
    var lastTouchEnd = 0;
    window.addEventListener('touchend', function (event) {
        var now = (new Date()).getTime();
        if (now - lastTouchEnd <= 300) {
            event.preventDefault();
        }
        lastTouchEnd = now;
    }, false);
    window.addEventListener('gesturestart', function (event) {  //禁止手势放大
        event.preventDefault();
    });
    window.addEventListener("orientationchange", function () {  //横竖屏监听
        if (window.orientation == 0) {
            document.getElementById("alert_rotate_layout").style.display = "";
        } else {
            document.getElementById("alert_rotate_layout").style.display = "none";
        }
    }, false);
}
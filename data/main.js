$(document).ready(function () {            
    $('.sidebarBth').click(function () {
        document.documentElement.webkitRequestFullscreen();
        $('.sidebar').toggleClass('sidebar-open');
        $('.sidebar').toggleClass('sidebar-close', false); 
        $('.contentbar').toggleClass('contentbar-close');
        $('.contentbar').toggleClass('contentbar-open', false);           
        $('.sidebarBth').toggleClass('toggle');                              
    });
    $('.item-1').click(function () {
        $('.sidebar').toggleClass('sidebar-close');
        $('.sidebar').toggleClass('sidebar-open', false);               
        $('.contentbar').toggleClass('contentbar-open');
        $('.contentbar').toggleClass('contentbar-close', false); 
        $('.sidebarBth').toggleClass('toggle');
        $('.contentbar').load('pid.html');                      
    });
    $('.item-2').click(function () {
        $('.sidebar').toggleClass('sidebar-close');
        $('.sidebar').toggleClass('sidebar-open', false);               
        $('.contentbar').toggleClass('contentbar-open');
        $('.contentbar').toggleClass('contentbar-close', false); 
        $('.sidebarBth').toggleClass('toggle');
        $('.contentbar').load('graph.html');    
    });
    $('.item-3').click(function () {
        $('.sidebar').toggleClass('sidebar-close');
        $('.sidebar').toggleClass('sidebar-open', false);               
        $('.contentbar').toggleClass('contentbar-open');
        $('.contentbar').toggleClass('contentbar-close', false); 
        $('.sidebarBth').toggleClass('toggle');
        $('.contentbar').load('dc.html');    
    });
    $('.item-4').click(function () {
        $('.sidebar').toggleClass('sidebar-close');
        $('.sidebar').toggleClass('sidebar-open', false);               
        $('.contentbar').toggleClass('contentbar-open');
        $('.contentbar').toggleClass('contentbar-close', false); 
        $('.sidebarBth').toggleClass('toggle'); 
        $('.contentbar').load('prog_1.html');  
    });   
})

var Temperature = new Array();
var Time = 0;
var Time_Step = 1000;

function get_temperatures () {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
            var data = this.responseText;
            data = data.replace(/'/g, '"');            
            data = JSON.parse(data);            
            for (let i = 0; i < data.length; i++) {
                Temperature[i] = parseFloat(data[i].value);
            }            
        }
    };
    xhttp.open('GET', '/temperatures', true);
    xhttp.send();
}

setTimeout(() => {get_temperatures();});

setInterval(() => {
    get_temperatures();    
    Time = Time + Time_Step * 1e-3;  
}, Time_Step);
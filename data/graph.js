var Trace = new Array();

Trace[0] = {   
    x: [Time],
    y: [Temperature[0]],
    mode: 'lines',
    name: 'T1'
};
  
Trace[1] = {   
    x: [Time],   
    y: [Temperature[1]],
    mode: 'lines',
    name: 'T2'
};

var light_color = '#ffffff';
var dark_color = '#272727';
var font_name = 'Times New Roman';
var size_font = 16;
var window_graph = {
    width: document.getElementById('content').offsetWidth,
    height: document.getElementById('content').offsetHeight
};

var layout = {
    title: {
        font:{
            family: font_name,
            color: light_color,
            size: 18
        },
        text: 'Температура термопар'
    },
    width: window_graph['width'],
    height: window_graph['height'],
    plot_bgcolor: dark_color,
    paper_bgcolor: dark_color,
    xaxis: {
        zerolinecolor: light_color,
        backgroundcolor: dark_color,
        gridcolor:light_color, 
        tickcolor: light_color,       
        tickfont: {
            color: light_color,
            size: size_font,
            family: font_name
        },
        title: {
            font:{
                family: font_name,
                color: light_color,
                size: size_font
            },
            text: 't [сек]'
        }
    },
    yaxis: {
        zerolinecolor: light_color,
        backgroundcolor: dark_color,
        gridcolor: light_color, 
        tickcolor: light_color,       
        tickfont: {
            color: light_color,
            size: size_font,
            family: font_name
        },
        title: {
            font:{
                family: font_name,
                color: light_color,
                size: size_font
            },
            text: 'T [град. Цельсия]'
        }
    },
    legend: {
        font: {
            color: light_color,
            size: size_font,
            family: font_name
        }
    }
};
  
var defaultPlotlyConfiguration = { 
    modeBarButtonsToRemove: ['sendDataToCloud', 'autoScale2d', 
    'hoverClosestCartesian', 'hoverCompareCartesian', 
    'lasso2d', 'select2d'], 
    displaylogo: false, showTips: true
};

Plotly.plot('graph', Trace, layout, defaultPlotlyConfiguration);

setInterval(function(){             
    Plotly.extendTraces(
        'graph',
        {      
            x: [[Time], [Time]],
            y: [[Temperature[0]], [Temperature[1]]]
        }, 
        [0, 1]
    );    
    if(Time > 10) {
        Plotly.relayout('graph', {
            xaxis: {
                zerolinecolor: light_color,
                backgroundcolor: dark_color,
                gridcolor:light_color, 
                tickcolor: light_color,       
                tickfont: {
                    color: light_color,
                    size: size_font,
                    family: font_name
                },
            title: {
                font:{
                    family: font_name,
                    color: light_color,
                    size: size_font
                },
                text: 't [сек]'
            },
            range: [Time - 10, Time + 5]
            }
        });
    }
}, Time_Step);
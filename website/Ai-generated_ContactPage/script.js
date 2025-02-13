
function toggleStyleSheet(){

    const styleHtml = document.getElementById("styleSheetInUse");
    const currentStyle = styleHtml.getAttribute("href");

    const lightCss = "./style.css";
    const darkCss = "./second.css";

    if (currentStyle == lightCss) { newCSS = darkCss;} 
    else{ newCSS = lightCss;}
    styleHtml.setAttribute("href", newCSS);

    localStorage.setItem("css", newCSS);
}


window.onload = function(){
    let newCSS = localStorage.getItem("css");
    
    if(!newCSS){
        newCSS = "./second.css";
        localStorage.setItem("css", newCSS);
    }

    const styleHtml = document.getElementById("styleSheetInUse");

    styleHtml.setAttribute("href", newCSS);
}
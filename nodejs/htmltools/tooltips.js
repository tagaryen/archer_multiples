/**
 * 绘制提示栏
 * @param {string} content 提示栏的内容
 * @param {{
 *   side:'top'|'bottom'|'left'|'right',
 *   left: number,
 *   top: number,
 *   backgroundColor: string
 * }} options 提示栏选项
*/
function drawToolTips(content, options) {
  if(!content) {
    content = "提示";
  }
  if(!options) {
    options = {};
  }
  let afterCssRule = null, side = options.side || 'top', left = options.left || 0, top = options.top || 0;
  let backgroundColor = options.backgroundColor || '#e2e8f0';
  let elId = `archer-tooltip-${side}`

  switch(side) {
    case 'top':
      afterCssRule = `#${elId}::after {content: ''; 
                                      position: absolute; 
                                      top: 100%; 
                                      left: 50%;
                                      transform: translateX(-50%) rotateX(180deg);
                                      border: 9px solid transparent;
                                      border-bottom-color: ${backgroundColor};
                                      pointer-events: none; }`
      break;
    case 'bottom':
      afterCssRule = `#${elId}::after {content: ''; 
                                      position: absolute; 
                                      bottom: 100%;
                                      left: 50%;
                                      transform: translateX(-50%) rotateX(180deg);
                                      border: 9px solid transparent;
                                      border-top-color: ${backgroundColor};
                                      pointer-events: none; }`
      break;
    case 'left':
      afterCssRule = `#${elId}::after {content: ''; 
                                      position: absolute; 
                                      top: 50%;
                                      left: 100%;
                                      transform: translateY(-50%) rotateY(180deg);
                                      border: 9px solid transparent;
                                      border-right-color: ${backgroundColor};  
                                      pointer-events: none; }`
      break;
    case 'right':
      afterCssRule = `#${elId}::before {content: ''; 
                                      position: absolute; 
                                      top: 50%;
                                      right: 100%;
                                      transform: translateY(-50%) rotateY(180deg);
                                      border: 9px solid transparent;
                                      border-left-color: ${backgroundColor};
                                      pointer-events: none; }`
      break;
  }
  let tooltip = document.getElementById(elId);
  if(tooltip) {
    tooltip.innerHTML = content;
    tooltip.style.display = 'inline-block';
    tooltip.style.left = left + 'px';
    tooltip.style.top = top + 'px';
    return;
  }
  if(afterCssRule) {
    tooltip = document.createElement('div');
    tooltip.id = elId;
    tooltip.innerHTML = content;
    tooltip.style.display = 'inline-block';
    tooltip.style.position = 'absolute';
    tooltip.style.zIndex = '1000';
    tooltip.style.left = left + 'px';
    tooltip.style.top = top + 'px';
    tooltip.style.display = 'inline-block';
    tooltip.style.padding = '4px 6px';
    tooltip.style.fontSize = '14px';
    tooltip.style.lineHeight = '1.5';
    tooltip.style.borderRadius = '8px';
    tooltip.style.backgroundColor = backgroundColor;
    tooltip.style.boxShadow = '0 8px 28px rgba(0, 0, 0, 0.18)';
    tooltip.style.textAlign = 'center';
    tooltip.style.transition = 'opacity 0.25s ease, transform 0.25s ease';
    let sheet = null;
    if(document.styleSheets.length > 0) {
      sheet = document.styleSheets[0];
    } else {
      let style = document.createElement('style');
      document.head.appendChild(style);
      sheet = document.styleSheets[0];
    }
    for(let rule of sheet.cssRules) {
      if (rule.selectorText === `#${elId}::after`) {
        return ;
      }
    }
    sheet.insertRule(afterCssRule, sheet.cssRules.length);
    document.body.appendChild(tooltip);
  }
}
/**
 * 移除提示栏
 * @param {'top'|'bottom'|'left'|'right'} side 提示栏的方向
*/
function removeToolTips(side) {
  let elId = `archer-tooltip-${side}`
  let tooltip = document.getElementById(elId);
  if(tooltip) {
    tooltip.style.display = 'none';
  }
}
// export default {drawToolTips, removeToolTips};
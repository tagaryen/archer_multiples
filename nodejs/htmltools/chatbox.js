const CSS = `
.aibox, .aibox * { box-sizing: border-box; margin: 0; padding: 0; }
.aibox {
  position: fixed;
  right: 0; bottom: 0;
  z-index: 99999;
  font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", "PingFang SC",
    "Hiragino Sans GB", "Microsoft YaHei", Roboto, Helvetica, Arial, sans-serif;
  font-size: 14px;
  line-height: 1.6;
}
.aibox button { font-family: inherit; cursor: pointer; }
.aibox__panel {
  position: fixed;
  right: 20px; bottom: 20px;
  width: 384px;
  max-width: calc(100vw - 32px);
  height: 580px;
  max-height: calc(100vh - 40px);
  display: flex;
  flex-direction: column;
  color: #e8eaf2;
  border: 1px solid rgba(255, 255, 255, 0.09);
  border-radius: 18px;
  box-shadow: 0 24px 64px rgba(0, 0, 0, 0.5), 0 0 0 1px rgba(255,255,255,0.02) inset;
  overflow: hidden;
  transition: transform 0.38s cubic-bezier(0.22, 1, 0.36, 1), opacity 0.3s ease;
}
.aibox--hidden .aibox__panel {
  transform: translateX(calc(100% + 48px));
  opacity: 0;
  pointer-events: none;
}
.aibox__header {
  display: flex;
  align-items: center;
  gap: 10px;
  padding: 14px 16px;
  background: linear-gradient(135deg, rgba(99, 102, 241, 0.25), rgba(56, 189, 248, 0.18));
  border-bottom: 1px solid rgba(255, 255, 255, 0.08);
  flex-shrink: 0;
}
.aibox__avatar {
  width: 34px; height: 34px;
  border-radius: 50%;
  display: flex; align-items: center; justify-content: center;
  font-size: 12px; font-weight: 700; color: #fff;
  background: linear-gradient(135deg, #6366f1, #22d3ee);
  box-shadow: 0 4px 14px rgba(99, 102, 241, 0.45);
  flex-shrink: 0;
}
.aibox__title-wrap { flex: 1; min-width: 0; }
.aibox__title {
  font-size: 14px; font-weight: 600;
  white-space: nowrap; overflow: hidden; text-overflow: ellipsis;
}
.aibox__subtitle {
  font-size: 11px;
  display: flex; align-items: center; gap: 5px;
}
.aibox__status-dot {
  width: 6px; height: 6px; border-radius: 50%;
  background: #34d399;
  box-shadow: 0 0 6px #34d399;
  animation: aibox-pulse 2s infinite;
}
.aibox__status-dot--busy { background: #fbbf24; box-shadow: 0 0 6px #fbbf24; }
@keyframes aibox-pulse {
  0%, 100% { opacity: 1; }
  50% { opacity: 0.4; }
}
.aibox__actions { display: flex; gap: 6px; }
.aibox__icon-btn {
  width: 28px; height: 28px;
  border: none; border-radius: 8px;
  background: rgba(255, 255, 255, 0.08);
  color: rgba(232, 234, 242, 0.75);
  font-size: 14px;
  display: flex; align-items: center; justify-content: center;
  transition: background 0.2s, color 0.2s;
}
.aibox__icon-btn:hover { background: rgba(255, 255, 255, 0.18); }
.aibox__messages {
  flex: 1;
  overflow-y: auto;
  padding: 16px 14px;
  display: flex;
  flex-direction: column;
  gap: 14px;
  scrollbar-width: thin;
  scrollbar-color: rgba(255,255,255,0.15) transparent;
}
.aibox__messages::-webkit-scrollbar { width: 6px; }
.aibox__messages::-webkit-scrollbar-thumb {
  background: rgba(255, 255, 255, 0.14);
  border-radius: 3px;
}
.aibox__messages::-webkit-scrollbar-track { background: transparent; }
.aibox__msg { display: flex; gap: 8px; animation: aibox-msg-in 0.3s ease both; }
@keyframes aibox-msg-in {
  from { opacity: 0; transform: translateY(8px); }
  to { opacity: 1; transform: translateY(0); }
}
.aibox__msg--user { flex-direction: row-reverse; }
.aibox__msg--tool { flex-direction: row; }
.aibox__msg-avatar {
  width: 26px; height: 26px;
  border-radius: 50%;
  flex-shrink: 0;
  margin-top: 2px;
  display: flex; align-items: center; justify-content: center;
  font-size: 10px; font-weight: 700; color: #fff;
  background: linear-gradient(135deg, #6366f1, #22d3ee);
}
.aibox__msg--user .aibox__msg-avatar {
  background: linear-gradient(135deg, #f472b6, #fb923c);
}
.aibox__bubble {
  max-width: 78%;
  padding: 9px 13px;
  border-radius: 14px;
  font-size: 13.5px;
  word-break: break-word;
  overflow-wrap: anywhere;
}
.aibox__msg--assistant .aibox__bubble {
  background: rgba(255, 255, 255, 0.07);
  border: 1px solid rgba(255, 255, 255, 0.06);
  border-top-left-radius: 4px;
}
.aibox__msg--user .aibox__bubble {
  background: linear-gradient(135deg, #6366f1, #4f46e5);
  color: #fff;
  border-top-right-radius: 4px;
  box-shadow: 0 4px 12px rgba(79, 70, 229, 0.35);
}
.aibox__msg--error .aibox__bubble {
  background: rgba(248, 113, 113, 0.12);
  border: 1px solid rgba(248, 113, 113, 0.35);
  color: #fca5a5;
}
.aibox__msg--tool .aibox__bubble {
  max-width: 88%;
  background: rgba(56, 189, 248, 0.07);
  border: 1px solid rgba(56, 189, 248, 0.22);
  border-radius: 12px;
  font-size: 12.5px;
  color: #bae6fd;
}
.aibox__tool-result {
  margin-top: 6px;
  padding: 7px 9px;
  background: rgba(0, 0, 0, 0.35);
  border-radius: 8px;
  font-family: Consolas, Monaco, "Courier New", monospace;
  font-size: 11.5px;
  color: #a5f3fc;
  white-space: pre-wrap;
  word-break: break-all;
  max-height: 110px;
  overflow-y: auto;
}
.aibox__bubble pre.aibox__code {
  background: #0b0f1a;
  border: 1px solid rgba(255, 255, 255, 0.08);
  border-radius: 8px;
  padding: 9px 11px;
  margin: 6px 0;
  overflow-x: auto;
  font-family: Consolas, Monaco, "Courier New", monospace;
  font-size: 12px;
  color: #a5f3fc;
  line-height: 1.5;
}
.aibox__bubble code.aibox__inline {
  background: rgba(255, 255, 255, 0.12);
  border-radius: 4px;
  padding: 1px 5px;
  font-family: Consolas, Monaco, "Courier New", monospace;
  font-size: 12px;
  color: #fde68a;
}
.aibox__dots { display: inline-flex; gap: 4px; align-items: center; height: 14px; }
.aibox__dots i {
  width: 5px; height: 5px;
  border-radius: 50%;
  background: rgba(232, 234, 242, 0.7);
  animation: aibox-bounce 1.2s infinite ease-in-out;
}
.aibox__dots i:nth-child(2) { animation-delay: 0.15s; }
.aibox__dots i:nth-child(3) { animation-delay: 0.3s; }
@keyframes aibox-bounce {
  0%, 60%, 100% { transform: translateY(0); opacity: 0.5; }
  30% { transform: translateY(-4px); opacity: 1; }
}
.aibox__caret {
  display: inline-block;
  width: 2px; height: 13px;
  margin-left: 2px;
  vertical-align: -2px;
  background: #22d3ee;
  animation: aibox-caret-blink 0.8s step-end infinite;
}
@keyframes aibox-caret-blink { 50% { opacity: 0; } }
.aibox__inputbar {
  display: flex;
  align-items: flex-end;
  gap: 8px;
  padding: 12px 14px;
  border-top: 1px solid rgba(255, 255, 255, 0.08);
  flex-shrink: 0;
}
.aibox__input {
  flex: 1;
  resize: none;
  border: 1px solid rgba(255, 255, 255, 0.12);
  border-radius: 12px;
  background: rgba(255, 255, 255, 0.06);
  color: #e8eaf2;
  font-family: inherit;
  font-size: 13.5px;
  line-height: 1.5;
  padding: 9px 12px;
  max-height: 110px;
  outline: none;
  transition: border-color 0.2s, background 0.2s;
  -ms-overflow-style: none;
  scrollbar-width: none; 
}
.aibox__input::-webkit-scrollbar {
  display: none; 
}
.aibox__input__dark::placeholder { color: rgba(244, 244, 244, 0.35); }
.aibox__input__light::placeholder { color: rgba(50, 50, 50, 0.35); }
.aibox__input:focus {
  border-color: rgba(99, 102, 241, 0.7);
  background: rgba(255, 255, 255, 0.09);
}
.aibox__send {
  width: 38px; height: 38px;
  border: none;
  border-radius: 12px;
  background: linear-gradient(135deg, #6366f1, #4f46e5);
  color: #fff;
  font-size: 15px;
  display: flex; align-items: center; justify-content: center;
  flex-shrink: 0;
  transition: transform 0.15s, filter 0.2s, opacity 0.2s;
  box-shadow: 0 4px 12px rgba(79, 70, 229, 0.4);
}
.aibox__send:hover:not(:disabled) { transform: translateY(-1px); filter: brightness(1.12); }
.aibox__send:active:not(:disabled) { transform: translateY(0); }
.aibox__send:disabled { opacity: 0.45; cursor: not-allowed; box-shadow: none; }
.aibox__launcher {
  position: fixed;
  right: 20px; bottom: 20px;
  width: 58px; height: 58px;
  border: none;
  border-radius: 50%;
  background: linear-gradient(135deg, #6366f1, #22d3ee);
  color: #fff;
  font-size: 13px; font-weight: 700;
  letter-spacing: 0.5px;
  display: flex; align-items: center; justify-content: center;
  box-shadow: 0 10px 28px rgba(99, 102, 241, 0.55);
  transition: transform 0.3s cubic-bezier(0.34, 1.56, 0.64, 1), opacity 0.25s;
  z-index: 1;
}
.aibox__launcher::after {
  content: "";
  position: absolute;
  inset: -4px;
  border-radius: 50%;
  border: 2px solid rgba(99, 102, 241, 0.5);
  animation: aibox-ring 2.4s infinite;
}
@keyframes aibox-ring {
  0% { transform: scale(0.9); opacity: 1; }
  100% { transform: scale(1.5); opacity: 0; }
}
.aibox__launcher:hover { transform: scale(1.08); }
.aibox:not(.aibox--hidden) .aibox__launcher {
  transform: scale(0);
  opacity: 0;
  pointer-events: none;
}
@media (max-width: 480px) {
  .aibox__panel {
    right: 8px; left: 8px; bottom: 8px;
    width: auto;
    height: calc(100vh - 90px);
  }
  .aibox__launcher { right: 14px; bottom: 14px; }
}
`;

const runJsexpression = {
  name: 'runJsexpression',
  description: 'Run a Javascript expression snippet to control a browser page. Only use this if other browser tools are insufficient. returns the result of the expression execution, maybe a string, maybe a number, maybe an object, etc.',
  parameters: {
    type: 'object',
    properties: { expression: { type: 'string', description: 'The Javascript expression to execute. The expression must be concise, serve one clear purpose.' } },
    required: ['expression'],
  },
  returns: {
    type: "any",
    description: "Return the result of the code execution. For example, if the code is \"document.title\", the result will be string, if it is \"1 + 2\", the result will be number, if it is \"Arrays.from(document.querySelectorAll('p')).map(el => {content: el.textContent})\", the result will be array, etc."
  },
  invoke: ({ expression }) => {
    if (!/^[\d+\-*/().\s%]+$/.test(expression)) {
      throw new Error('仅支持数字与 + - * / % ( ) 运算符');
    }
    return `${expression} = ${Function('"use strict"; return (' + expression + ')')()}`;
  },
};

function injectStyles() {
  if (document.getElementById('aibox-style')) return;
  const style = document.createElement('style');
  style.id = 'aibox-style';
  style.textContent = CSS;
  document.head.appendChild(style);
}
function escapeHtml(s) {
  return String(s)
    .replace(/&/g, '&amp;')
    .replace(/</g, '&lt;')
    .replace(/>/g, '&gt;')
    .replace(/"/g, '&quot;');
}

function generateSecureRandomString(length = 32) {
    const chars = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789';
    const randomValues = new Uint8Array(length);
    window.crypto.getRandomValues(randomValues);
    let result = '';
    for (let i = 0; i < length; i++) {
        result += chars[randomValues[i] % chars.length];
    }
    return result;
}

const DEFAULT_SYSTEM_MESSAGES = [
    {
      "role": "system",
      "content": "You are an expert AI assistant.\nWhen asked for your name, you must respond with \"Archer AI\".\nFollow the user's requirements carefully & to the letter.\nKeep your answers short and impersonal.\nYou are a highly sophisticated automated agent with expert-level knowledge.\nThe user will ask a question, or ask you to perform a task, and it may require lots of research to answer correctly. There is a selection of tools that let you perform actions or retrieve helpful context to answer the user's question.\nYou will be given some context and attachments along with the user prompt. You can use them if they are relevant to the task, and ignore them if not.\nIf you aren't sure which tool is relevant, you can call multiple tools. You can call tools repeatedly to take actions or gather as much context as needed until you have completed the task fully. Don't give up unless you are sure the request cannot be fulfilled with the tools you have. It's YOUR RESPONSIBILITY to make sure that you have done all you can to collect necessary context.\nDon't repeat yourself after a tool call, pick up where you left off.\nNEVER print out a codeblock with file changes unless the user asked for it. Use the appropriate edit tool instead.\nNEVER print out a codeblock with a terminal command to run unless the user asked for it.\n<toolUseInstructions>\nWhen using a tool, follow the JSON schema very carefully and make sure to include ALL required properties.\nNo need to ask permission before using a tool.\nNEVER say the name of a tool to a user. For example, instead of saying that you'll use the tool click_button, say \"I'll click the xxx button\".\nIf you think running multiple tools can answer the user's question, prefer calling them in parallel whenever possible.\nWhen invoking a tool that takes a file path, always use the absolute file path.\nTools can be disabled by the user. You may see tools used previously in the conversation that are not currently available. Be careful to only use the tools that are currently available to you.\n</toolUseInstructions>"
    }
];

function parseArguments(args) {
	if (!args || typeof args !== 'string') {
		return {};
	}
	try {
		return JSON.parse(args);
	} catch {
		throw new Error(`Invalid JSON arguments for tool call: ${args.slice(0, 200)}`);
	}
}

function resultToString(result) {
	if (typeof result === 'string') {
		return result;
	}
	return JSON.stringify(result);
}

class AgentSession {
	/**
	 * @param {object} opts
	 * @param {string} opts.baseUrl LLM API 基础 URL。
	 * @param {string} opts.apiKey LLM API 密钥。
	 * @param {string} opts.model LLM 模型名称。
	 * @param {number} [opts.maxTurns] 最大工具调用轮数，默认 15。
	 */
	constructor({ baseUrl, apiKey, model,  maxTurns = 15 }) {
		this._baseUrl = baseUrl;
		this._apiKey = apiKey;
		this._model = model;
    this.maxTurns = maxTurns;
		this._tools = new Map();
		this._toolDefs = [];
		this._resetMessages();
	}
  _resetMessages() {
    this.messages = DEFAULT_SYSTEM_MESSAGES;
    this.messages.push({
      role: 'user',
      content: `User current enviroment is Browser.User current time is ${new Date().toLocaleString()}`,
    });
  }
  _appendUserMessage(userInput) {
    this.messages.push({ 
      role: 'user', 
      content: userInput
    });
  }
  _appendAssitantMessage(inputText, toolCalls) {
    this.messages.push({ 
      role: 'assistant', 
      content: inputText,
      ...(toolCalls.length ? {
        tool_calls: toolCalls.map(c => ({
          id: c.id,
          type: 'function',
          function: { name: c.name, arguments: c.arguments },
        })),
      } : {}) });
  }

  _appendToolMessage(call, result) {
    this.messages.push({
      role: 'tool',
      tool_call_id: call.id,
      content: result,
    });
  }
	
	async _invoke(name, args, ctx) {
		const tool = this._tools.get(name);
		if (!tool) {
			throw new Error(`Tool "${name}" is not registered`);
		}
		const result = await tool.invoke(args || {}, ctx || {});
		if (result === undefined || result === null) {
			return '';
		}
		if (typeof result === 'string') {
			return result;
		}
		return typeof result === 'object' ? JSON.stringify(result) : String(result);
	}

	async _chatToLLM({ messages, tools, onDelta }) {
		const body = {
			model: this._model,
			messages,
			stream: true,
		};
		if (tools?.length) {
			body.tools = tools;
		}
		if (this.temperature !== undefined) {
			body.temperature = this.temperature;
		}
		const headers = {
			'Content-Type': 'application/json',
			'Authorization': `Bearer ${this._apiKey}`,
			'X-Request-Id': generateSecureRandomString(),
			'OpenAI-Intent': 'conversation-agent',
			'X-Interaction-Type': 'conversation-agent',
		};
		const doFetch = () => fetch(`${this._baseUrl}/chat/completions`, {
			method: 'POST',
			headers,
			body: JSON.stringify(body),
		});
		let response;
		try {
			response = await doFetch();
		} catch (err) {
			try {
				response = await doFetch();
			} catch (err) {
        console.log(err);
        this.messages.pop();
        return {
          content: "目前功能暂时不可用",
          toolCalls: []
        };
			}
		}

		if (!response.ok) {
			const text = await response.text().catch(() => '');
			throw new Error(`LLM request failed: ${response.status} ${response.statusText} ${text.slice(0, 500)}`);
		}
		
		let content = '';
		const toolCalls = [];
		let finishReason = '';
		let usage;

		const reader = response.body.getReader();
		const decoder = new TextDecoder();
		let buffer = '';

		const handleEvent = (data) => {
			if (data === '[DONE]') {
				return true;
			}
			let chunk;
			try {
				chunk = JSON.parse(data);
			} catch {
				return false;
			}
			if (chunk.usage) {
				usage = chunk.usage;
			}
			const choice = chunk.choices?.[0];
			if (!choice) {
				return false;
			}
			const delta = choice.delta ?? {};
			if (typeof delta.content === 'string' && delta.content) {
				content += delta.content;
				onDelta?.(delta.content);
			}
			for (const tc of delta.tool_calls ?? []) {
				const i = tc.index ?? 0;
				toolCalls[i] ??= { id: '', name: '', arguments: '' };
				if (tc.id) {
					toolCalls[i].id = tc.id;
				}
				if (tc.function?.name) {
					toolCalls[i].name = tc.function.name;
				}
				if (tc.function?.arguments) {
					toolCalls[i].arguments += tc.function.arguments;
				}
			}
			if (choice.finish_reason) {
				finishReason = choice.finish_reason;
			}
			return false;
		};

		while (true) {
			const { done, value } = await reader.read();
			if (done) {
				break;
			}
			buffer += decoder.decode(value, { stream: true });
			let idx;
			while ((idx = buffer.indexOf('\n')) >= 0) {
				const line = buffer.slice(0, idx).replace(/\r$/, '');
				buffer = buffer.slice(idx + 1);
				if (line.startsWith('data:')) {
					if (handleEvent(line.slice(5).trim())) {
						buffer = '';
					}
				}
			}
		}
		if (buffer.startsWith('data:')) {
			handleEvent(buffer.slice(5).trim());
		}

		return {
			content,
			toolCalls: toolCalls.filter(Boolean),
			finishReason,
			usage,
		};
	}
	registerTool(tool) {
		if (!tool || !tool.name) {
			throw new Error('Tool must have a name');
		}
		this._tools.set(tool.name, tool);
		this._toolDefs.push({
			type: 'function',
			function: {
				name: tool.name,
				description: tool.description,
				parameters: tool.parameters,
        returns: tool.returns
			},
		});
	}
	listTools() {
		return this._toolDefs;
	}
	async callTool(call) {
		let result;
		try {
			result = await this._invoke(call.name, parseArguments(call.arguments), call.ctx);
		} catch (err) {
			result = `Error: ${err.message}`;
		}
    return resultToString(result);
	}
	async chat(userInput, handlers = {}) {
    if(!this._baseUrl || !this._apiKey || !this._model) {
		  return { content: '请先设置好AI引擎相关的接口地址等数据' };  
    }
		this._appendUserMessage(userInput);
		for (let turn = 0; turn < this.maxTurns; turn++) {
			const res = await this._chatToLLM({
				messages: this.messages,
				tools: this._toolDefs,
				onDelta: handlers.onDelta,
			});
      this._appendAssitantMessage(res.content || null, res.toolCalls);
			if (!res.toolCalls.length) {
				return { content: res.content };
			}

			for (const call of res.toolCalls) {
				handlers.onToolCall?.(call);
				let result;
				try {
          result = await this._invoke(call.name, parseArguments(call.arguments), call.ctx);
				} catch (err) {
					result = `Error: ${err.message}`;
				}
				handlers.onToolResult?.(call, resultToString(result));
                this._appendToolMessage(call, result);
			}
		}

		const msg = 'Reached max tool rounds without a final answer.';
		return { content: msg };
	}
}

class ChatBox {
  /**
   * @param {object} opts
   * @param {object} opts.baseUrl          LLMClient 实例（由外部传入）
   * @param {Function} opts.apiKey  AgentSession 类（由外部传入）
   * @param {Function} opts.model   ToolFactory 类（由外部传入）
   * @param {Array<object>} [opts.tools]  工具列表（ToolFactory.registerTool 格式）
   * @param {string} [opts.title]        面板标题
   * @param {boolean} [opts.hidden]      初始是否隐藏（默认展开）
   */
  constructor(opts = {}) {
    const {
      baseUrl,
      apiKey,
      model,
      tools = [],
      title = 'AI 助手',
      hidden = false,
      darkMode = false,
    } = opts;
    this._session = new AgentSession({
      baseUrl,
      apiKey,
      model,
    });
    this._darkMode = darkMode;
    this._tools = tools;
    this._title = title;
    this._subtitle = '在线';
    this._welcome = '你好！我是你的 **AI 助手**，有什么可以帮你？';
    this._placeholder = '输入消息，Enter 发送…';
    this._busy = false;

    this._tools.unshift(runJsexpression);
    for (const tool of this._tools) {
      this._session.registerTool(tool);
    }

    this._buildDOM();
    this._bindEvents();
    this._newSession();
    if (hidden) this.hide();
  }

  _buildDOM() {
    injectStyles();
    let bgColor, titleColor, subTittleColor, inputBgColor;
    if(this._darkMode) {
      bgColor = 'linear-gradient(180deg, #1b2032 0%, #121624 100%)';
      titleColor = '#fff';
      subTittleColor = 'rgba(232, 234, 242, 0.55);';
      inputBgColor = 'rgba(0, 20, 42, 0.2)';
    }  else {
      bgColor = 'linear-gradient(180deg, #f2f4f9ff 0%, #dfe3f5ff 100%)';
      titleColor = '#000';
      subTittleColor = 'rgba(105, 107, 112, 0.55)';
      inputBgColor = 'rgba(0, 63, 199, 0.2)';
    }
    this._root = document.createElement('div');
    this._root.className = 'aibox';
    this._root.innerHTML = `
      <div class="aibox__panel" style="background: ${bgColor};" role="dialog" aria-label="${escapeHtml(this._title)}">
        <header class="aibox__header">
          <div class="aibox__avatar">AI</div>
          <div class="aibox__title-wrap">
            <div class="aibox__title" style="color: ${titleColor};">${escapeHtml(this._title)}</div>
            <div class="aibox__subtitle" style="color: ${subTittleColor};">
              <span class="aibox__status-dot"></span>
              <span class="aibox__subtitle-text">${escapeHtml(this._subtitle)}</span>
            </div>
          </div>
          <div class="aibox__actions">
            <button class="aibox__icon-btn" style="color: ${titleColor};" data-action="reset" title="清空对话">↺</button>
            <button class="aibox__icon-btn" style="color: ${titleColor};" data-action="hide" title="隐藏到右侧">➜</button>
          </div>
        </header>
        <div class="aibox__messages"></div>
        <div class="aibox__inputbar" style="background-color: ${inputBgColor};">
          <textarea class="aibox__input" rows="1" placeholder="${escapeHtml(this._placeholder)}"></textarea>
          <button class="aibox__send" title="发送">➤</button>
        </div>
      </div>
      <button class="aibox__launcher" title="打开 AI 助手">AI</button>`;
    this._panel = this._root.querySelector('.aibox__panel');
    this._messagesEl = this._root.querySelector('.aibox__messages');
    this._input = this._root.querySelector('.aibox__input');
    this._input.classList.add(this._darkMode ? 'aibox__input__dark' : 'aibox__input__light');
    this._sendBtn = this._root.querySelector('.aibox__send');
    this._launcher = this._root.querySelector('.aibox__launcher');
    this._statusDot = this._root.querySelector('.aibox__status-dot');
    this._subtitleEl = this._root.querySelector('.aibox__subtitle-text');

    document.body.appendChild(this._root);
  }

  _bindEvents() {
    this._sendBtn.addEventListener('click', () => this._submit());
    this._input.addEventListener('keydown', (e) => {
      if (e.key === 'Enter' && !e.shiftKey && !e.isComposing) {
        e.preventDefault();
        this._submit();
      }
    });
    this._input.addEventListener('input', () => this._autoGrow());
    this._launcher.addEventListener('click', () => this.show());
    this._root.querySelector('[data-action="hide"]').addEventListener('click', () => this.hide());
    this._root.querySelector('[data-action="reset"]').addEventListener('click', () => this.reset());
    document.addEventListener('keydown', (e) => {
      if (e.key === 'Escape' && !this._root.classList.contains('aibox--hidden')) this.hide();
    });
  }
  _newSession() {
    this._session._resetMessages();
  }
  _addWelcome() {
    if (!this._welcome) return;
    this._addMessage('assistant', this._renderMarkdown(this._welcome));
  }
  _addMessage(role, html) {
    let messageColor, msgBgColor;
    if(!this._darkMode) {
      messageColor = '#000';
      msgBgColor = '#c6daf7ff';
    }  else {
      messageColor = '#fff';
      msgBgColor = '#7395c5ff';
    }
    const el = document.createElement('div');
    el.className = `aibox__msg aibox__msg--${role}`;
    const avatarLabel = role === 'user' ? '我' : 'AI';
    el.innerHTML = `
      ${role === 'tool' ? '' : `<div class="aibox__msg-avatar">${avatarLabel}</div>`}
      <div class="aibox__bubble" style="background-color: ${msgBgColor};color: ${messageColor};"><div class="aibox__content"></div></div>
    `;
    el.__content = el.querySelector('.aibox__content');
    el.__content.innerHTML = html;
    this._messagesEl.appendChild(el);
    this._scrollToEnd();
    return el;
  }

  _scrollToEnd() {
    this._messagesEl.scrollTop = this._messagesEl.scrollHeight;
  }
  _renderMarkdown(text) {
    const parts = String(text ?? '').split('```');
    let html = '';
    parts.forEach((part, i) => {
      if (i % 2 === 1) {
        const code = part.replace(/^\w*\r?\n/, '');
        html += `<pre class="aibox__code"><code>${escapeHtml(code)}</code></pre>`;
      } else {
        let t = escapeHtml(part);
        t = t.replace(/`([^`\n]+)`/g, '<code class="aibox__inline">$1</code>');
        t = t.replace(/\*\*([^*\n]+)\*\*/g, '<strong>$1</strong>');
        t = t.replace(/(^|[^*])\*([^*\n]+)\*(?!\*)/g, '$1<em>$2</em>');
        t = t.replace(/(^|\n)\s*[-*]\s+/g, '$1• ');
        t = t.replace(/\n/g, '<br>');
        html += t;
      }
    });
    return html;
  }
  _autoGrow() {
    this._input.style.height = 'auto';
    this._input.style.height = Math.min(this._input.scrollHeight, 110) + 'px';
  }

  _setBusy(busy) {
    this._busy = busy;
    this._sendBtn.disabled = busy;
    this._statusDot.classList.toggle('aibox__status-dot--busy', busy);
    this._subtitleEl.textContent = busy ? '思考中…' : this._subtitle;
  }

  _submit() {
    if (this._busy) return;
    const text = this._input.value.trim();
    if (!text) return;
    this._input.value = '';
    this._autoGrow();
    this._send(text);
  }

  async _send(text) {
    this._addMessage('user', escapeHtml(text));
    this._setBusy(true);

    const bubble = this._addMessage('assistant', '<span class="aibox__dots"><i></i><i></i><i></i></span>');
    bubble.__content.classList.add('aibox__streaming');

    let acc = '';
    let firstDelta = true;
    const toolChips = new Map();

    try {
      const res = await this._session.chat(text, {
        onDelta: (delta) => {
          if (firstDelta) {
            firstDelta = false;
            bubble.__content.innerHTML = '';
          }
          acc += delta;
          bubble.__content.innerHTML =
            this._renderMarkdown(acc) + '<span class="aibox__caret"></span>';
          this._scrollToEnd();
        },
        onToolCall: (call) => {
          const chip = this._addMessage(
            'tool',
            `🔧 正在调用工具 <strong>${escapeHtml(call.name)}</strong> …`
          );
          chip.dataset.callId = call.id;
          toolChips.set(call.id, chip);
        },
        onToolResult: (call, result) => {
          const chip = toolChips.get(call.id);
          if (!chip) return;
          const preview = String(result ?? '');
          const short = preview.length > 400 ? preview.slice(0, 400) + ' …' : preview;
          chip.__content.innerHTML =
            `🔧 工具 <strong>${escapeHtml(call.name)}</strong> 执行完成` +
            (short ? `<div class="aibox__tool-result">${escapeHtml(short)}</div>` : '');
          this._scrollToEnd();
        },
      });
      if (firstDelta && res?.content) {
        bubble.__content.innerHTML = this._renderMarkdown(res.content);
      }
    } catch (err) {
      bubble.classList.add('aibox__msg--error');
      bubble.__content.innerHTML = `⚠ ${escapeHtml(err?.message || '请求失败，请稍后重试')}`;
    } finally {
      const caret = bubble.__content.querySelector('.aibox__caret');
      if (caret) caret.remove();
      bubble.__content.classList.remove('aibox__streaming');
      if (!bubble.__content.textContent.trim() && !bubble.__content.querySelector('pre')) {
        bubble.__content.innerHTML = '<em style="opacity:.6">（无响应内容）</em>';
      }
      this._setBusy(false);
      this._scrollToEnd();
    }
  }

  reset() {
    if (this._busy) return;
    this._messagesEl.innerHTML = '';
    this._newSession();
    this._addWelcome();
  }
  show() {
    this._root.classList.remove('aibox--hidden');
    this._input.focus();
  }

  hide() {
    this._root.classList.add('aibox--hidden');
  }

  toggle() {
    this._root.classList.contains('aibox--hidden') ? this.show() : this.hide();
  }
}

export {AgentSession, ChatBox};
// export default {AgentSession, ChatBox};

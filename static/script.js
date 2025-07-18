document.addEventListener('DOMContentLoaded', function() {
    const loginForm = document.getElementById('login-form');
    const messageForm = document.getElementById('message-form');
    const toggleRegister = document.getElementById('toggle-register');
    
    // Проверка авторизации при загрузке
    function checkAuth() {
        return document.cookie.includes('auth_token');
    }

    // Функция для обработки входа
    function handleLogin(login, password) {
        return fetch('/api/login', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json'
            },
            body: JSON.stringify({
                login: login,
                password: password
            })
        })
        .then(async response => {
            const data = await response.json();
            if (!response.ok) {
                throw new Error(data.error || data.message || 'Login failed');
            }
            return data;
        });
    }

    if (loginForm) {
        let isRegister = false;
        
        if (toggleRegister) {
            toggleRegister.addEventListener('click', function(e) {
                e.preventDefault();
                isRegister = !isRegister;
                
                const title = document.querySelector('h1');
                const submitBtn = loginForm.querySelector('button');
                const toggleText = toggleRegister;
                
                if (isRegister) {
                    title.textContent = 'Register';
                    submitBtn.textContent = 'Register';
                    toggleText.textContent = 'Login';
                } else {
                    title.textContent = 'Login';
                    submitBtn.textContent = 'Login';
                    toggleText.textContent = 'Register';
                }
            });
        }
        
        loginForm.addEventListener('submit', function(e) {
            e.preventDefault();
            
            const login = document.getElementById('login').value;
            const password = document.getElementById('password').value;
            const errorElement = document.getElementById('error');
            
            const endpoint = isRegister ? '/api/register' : '/api/login';
            
            errorElement.textContent = '';
            
            if (isRegister) {
                // Обработка регистрации
                fetch('/api/register', {
                    method: 'POST',
                    headers: {
                        'Content-Type': 'application/json'
                    },
                    body: JSON.stringify({
                        login: login,
                        password: password
                    })
                })
                .then(async response => {
                    const data = await response.json();
                    if (!response.ok) {
                        throw new Error(data.error || data.message || 'Registration failed');
                    }
                    // После успешной регистрации автоматически входим
                    return handleLogin(login, password);
                })
                .then(() => {
                    window.location.href = '/';
                })
                .catch(error => {
                    errorElement.textContent = error.message;
                });
            } else {
                // Обработка входа
                handleLogin(login, password)
                    .then(() => {
                        window.location.href = '/';
                    })
                    .catch(error => {
                        errorElement.textContent = error.message;
                    });
            }
        });
        
    }
    
    if (messageForm) {
        const messagesContainer = document.getElementById('messages');
        const messageInput = document.getElementById('message-input');
        const logoutBtn = document.getElementById('logout-btn');

        function loadMessages() {
            fetch('/api/messages')
                .then(response => response.json())
                .then(data => {
                    messagesContainer.innerHTML = '';
                    data.messages.forEach(msg => {
                        const messageElement = document.createElement('div');
                        messageElement.className = 'message';
                        messageElement.innerHTML =
                            '<span class="user">' + msg.user + '</span>' +
                            '<span class="time">' + msg.time + '</span>' +
                            '<div>' + msg.text + '</div>';
                        messagesContainer.appendChild(messageElement);
                    });
                    messagesContainer.scrollTop = messagesContainer.scrollHeight;
                });
        }
        
        // Обработчик отправки сообщения
        messageForm.addEventListener('submit', async function(e) {
        e.preventDefault();
    
        const errorElement = document.getElementById('message-error') || createErrorElement();
        errorElement.textContent = '';
    
    try {
        const response = await fetch('/api/chat', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
                'Authorization': `Bearer ${getCookie('auth_token')}`
            },
            body: JSON.stringify({
                message: messageInput.value.trim()
            })
        });

        const data = await response.json();
        
        if (!response.ok) {
            if (data.action === 'register') {
                errorElement.innerHTML = `${data.message}. <a href="/login">Перейти к регистрации</a>`;
            } else {
                errorElement.textContent = data.error || 'Ошибка отправки сообщения';
            }
            return;
        }

        messageInput.value = '';
        loadMessages();
        
    } catch (error) {
        errorElement.textContent = 'Ошибка соединения с сервером';
        console.error(error);
    }
});

        messageForm.addEventListener('submit', function(e) {
            e.preventDefault();
            
            const message = messageInput.value.trim();
            if (message === '') return;
            
            fetch('/api/chat', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json'
                },
                body: JSON.stringify({
                    message: message
                })
            })
            .then(response => {
                if (!response.ok) {
                    throw new Error('Failed to send message');
                }
                messageInput.value = '';
                loadMessages();
            })
            .catch(error => {
                console.error(error);
            });
        });
        
        if (logoutBtn) {
            logoutBtn.addEventListener('click', function() {
                document.cookie = 'auth_token=; Path=/; Expires=Thu, 01 Jan 1970 00:00:01 GMT;';
                window.location.href = '/login';
            });
        }
        // Обработчик выхода
logoutBtn.addEventListener('click', async function() {
    try {
        const response = await fetch('/api/logout', {
            method: 'POST',
            credentials: 'include'
        });
        
        // Принудительная очистка на клиенте
        document.cookie.split(';').forEach(cookie => {
            document.cookie = cookie.trim().split('=')[0] + 
                '=; Path=/; Expires=Thu, 01 Jan 1970 00:00:00 GMT';
        });
        
        // Перенаправление с очисткой кеша
        window.location.replace('/login');
        
    } catch (error) {
        console.error('Ошибка выхода:', error);
        window.location.href = '/login';
    }
    // Вспомогательная функция для создания элемента ошибки
function createErrorElement() {
    const element = document.createElement('div');
    element.id = 'message-error';
    element.style.color = 'red';
    element.style.margin = '10px 0';
    messageForm.parentNode.insertBefore(element, messageForm.nextSibling);
    return element;
}
});
        loadMessages();
        setInterval(loadMessages, 3000);
    }
});
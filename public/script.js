document.addEventListener('DOMContentLoaded', () => {
    const status = document.getElementById('js-status');
    if (status) {
        status.textContent = 'JavaScript loaded successfully! Server is working.';
        status.style.color = '#27ae60';
        status.style.fontWeight = 'bold';
    }
    console.log('chttpd static assets loaded.');
});

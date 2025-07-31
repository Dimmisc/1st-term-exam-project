
document.addEventListener('DOMContentLoaded', () => {

    const EDITABLE_TAGS = ['p', 'h1', 'h2', 'h3', 'h4', 'h5', 'h6', 'li', 'blockquote', 'span'];
    const selector = EDITABLE_TAGS.join(',');
    function getElementKey(element) {
        let path = '';
        let current = element;
        while (current && current.parentElement) {
            const parent = current.parentElement;
            const index = Array.from(parent.children).indexOf(current);
            const tagName = current.tagName.toLowerCase();
            path = `/${tagName}[${index}]` + path;
            if (parent.tagName.toLowerCase() === 'body') break;
            current = parent;
        }
        return path;
    }

    function saveElementContent(event) {
        const element = event.target;
        const key = getElementKey(element);
        localStorage.setItem(key, element.innerHTML);
    }
    const elementsToEdit = document.querySelectorAll(selector);
    elementsToEdit.forEach(el => {
        const key = getElementKey(el);
        const savedContent = localStorage.getItem(key);
        if (savedContent) {
            el.innerHTML = savedContent;
        }
        el.contentEditable = true;
        el.classList.add('editable-permanent');
        el.addEventListener('blur', saveElementContent);
    });

});

function generatePDF() {
                const downloadButton = document.getElementById('download-button');
                downloadButton.style.display = 'none';
                const element = document.getElementById('page-to-download');
                element.style.width = '555px';
                const opt = {
                    margin: [0, 0, 0, 0],
                    filename: 'document.pdf',
                    image: { type: 'jpeg', quality: 0.98 },
                    html2canvas: { scale: 6, useCORS: true },
                    jsPDF: { unit: 'px', format: [element.offsetWidth + 100, element.offsetHeight + 100], orientation: 'portrait' }
                };

                html2pdf().set(opt).from(element).save().then(() => {
                    downloadButton.style.display = 'block'; 
                });
                element.style.width = '';
                downloadButton.style.display = 'block';
            }
document.getElementById('download-button').addEventListener('click', generatePDF);
document.getElementById('reset-button').addEventListener('click', localStorage.clear());
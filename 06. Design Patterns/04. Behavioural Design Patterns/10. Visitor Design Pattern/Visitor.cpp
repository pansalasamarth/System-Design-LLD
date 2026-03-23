#include <iostream>
#include <string>
#include <vector>
#include <memory>

using namespace std;

// Forward declarations for all file types
class TextFile;
class ImageFile;
class VideoFile;

// Visitor Interface
class FileSystemVisitor {
public:
    virtual ~FileSystemVisitor() = default;
    
    virtual void visit(TextFile* file) = 0;
    virtual void visit(ImageFile* file) = 0;
    virtual void visit(VideoFile* file) = 0;
};

class FileSystemItem {
protected:
    string name;
    
public:
    FileSystemItem(const string& itemName) {
        name = itemName;
    }
    virtual ~FileSystemItem() = default;
    
    string getName() const { return name; }
    
    virtual void accept(FileSystemVisitor* visitor) = 0;
};

class TextFile : public FileSystemItem {
private:
    string content;
    
public:
    TextFile(const string& fileName, const string& fileContent) : FileSystemItem(fileName) {
        content = fileContent;
    }
    
    string getContent() const { 
        return content; 
    }
    
    void accept(FileSystemVisitor* visitor) override {
        visitor->visit(this);  
    }
};

class ImageFile : public FileSystemItem {
    
public:
    ImageFile(string fileName) : FileSystemItem(fileName) {}
    
    void accept(FileSystemVisitor* visitor) override {
        visitor->visit(this); 
    }
};

class VideoFile : public FileSystemItem {
public:
    VideoFile(const string& fileName) : FileSystemItem(fileName) {}
    
    void accept(FileSystemVisitor* visitor) override {
        visitor->visit(this); 
    }
};

// 1. Size calculation visitor
class SizeCalculationVisitor : public FileSystemVisitor {
public:
    void visit(TextFile* file) override {
        cout << "Calculating size for TEXT file: " << file->getName() << endl;
    }
    
    void visit(ImageFile* file) override {
        cout << "Calculating size for IMAGE file: " << file->getName() << endl;
    }
    
    void visit(VideoFile* file) override {
        cout << "Calculating size for VIDEO file: " <<  file->getName() << endl;
    }
};

// 2. Compression Visitor
class CompressionVisitor : public FileSystemVisitor {
public:
    void visit(TextFile* file) override {
        cout << "Compressing TEXT file: " << file->getName() << endl;
    }
    
    void visit(ImageFile* file) override {
        cout << "Compressing IMAGE file: " << file->getName() << endl;
    }
    
    void visit(VideoFile* file) override {
        cout << "Compressing VIDEO file: " << file->getName() << endl;
    }
};

// 3. Virus Scanning Visitor
class VirusScanningVisitor : public FileSystemVisitor {
public:
    void visit(TextFile* file) override {
        cout << "Scanning TEXT file: " << file->getName() << endl;
    }
    
    void visit(ImageFile* file) override {
        cout << "Scanning IMAGE file: " << file->getName() << endl;
    }
    
    void visit(VideoFile* file) override {
        cout << "Scanning VIDEO file: " << file->getName() << endl;
    }
};

int main() {

    FileSystemItem* img1 = new ImageFile("sample.jpg");

    img1->accept(new SizeCalculationVisitor());
    img1->accept(new CompressionVisitor());
    img1->accept(new VirusScanningVisitor());

    FileSystemItem* vid1 = new VideoFile("test.mp4");
    vid1->accept(new CompressionVisitor());
    
    return 0;
}
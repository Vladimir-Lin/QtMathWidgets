/****************************************************************************
 *                                                                          *
 * Copyright (C) 2001 ~ 2016 Neutrino International Inc.                    *
 *                                                                          *
 * Author : Brian Lin <lin.foxman@gmail.com>, Skype: wolfram_lin            *
 *                                                                          *
 ****************************************************************************/

#ifndef QT_MATHWIDGETS_H
#define QT_MATHWIDGETS_H

#include <QtManagers>

QT_BEGIN_NAMESPACE

#ifndef QT_STATIC
#    if defined(QT_BUILD_MATHWIDGETS_LIB)
#      define Q_MATHWIDGETS_EXPORT Q_DECL_EXPORT
#    else
#      define Q_MATHWIDGETS_EXPORT Q_DECL_IMPORT
#    endif
#else
#    define Q_MATHWIDGETS_EXPORT
#endif

namespace Ui                              {
class Q_MATHWIDGETS_EXPORT nEditMatrix    ;
}

namespace N
{

class Q_MATHWIDGETS_EXPORT PrimeEditor    ;
class Q_MATHWIDGETS_EXPORT SetMemberLists ;
class Q_MATHWIDGETS_EXPORT SetLists       ;
class Q_MATHWIDGETS_EXPORT SetsRules      ;
class Q_MATHWIDGETS_EXPORT NumSeries      ;
class Q_MATHWIDGETS_EXPORT RuleBases      ;
class Q_MATHWIDGETS_EXPORT DecisionLists  ;
class Q_MATHWIDGETS_EXPORT DecisionEditor ;
class Q_MATHWIDGETS_EXPORT ActionLists    ;
class Q_MATHWIDGETS_EXPORT ConditionLists ;
class Q_MATHWIDGETS_EXPORT MatrixEditor   ;
class Q_MATHWIDGETS_EXPORT EditMatrix     ;
class Q_MATHWIDGETS_EXPORT MatricesLists  ;

/*****************************************************************************
 *                                                                           *
 *                                Math Widgets                               *
 *                                                                           *
 *****************************************************************************/

class Q_MATHWIDGETS_EXPORT PrimeEditor : public Widget
{
  Q_OBJECT
  public:

    explicit PrimeEditor     (StandardConstructor) ;
    virtual ~PrimeEditor     (void);

  protected:

    SUID          PrimeId ;
    QSpinBox    * ID      ;
    QSpinBox    * P       ;
    QTreeWidget * Prime   ;

    virtual void showEvent   (QShowEvent * event) ;
    virtual void resizeEvent (QResizeEvent * event) ;

    virtual bool FocusIn     (void) ;

    virtual void run         (int Type,ThreadData * data) ;
    virtual bool Load        (ThreadData * data) ;

  private:

  public slots:

    virtual bool startup     (void) ;
    virtual void Reload      (void) ;
    virtual void Relocation  (void) ;

  protected slots:

    void primeChanged        (int v);
    void valueChanged        (int v);

  private slots:

  signals:

} ;

class Q_MATHWIDGETS_EXPORT SetMemberLists : public TreeDock
                                          , public CodeManager
{
  Q_OBJECT
  public:

    explicit SetMemberLists        (StandardConstructor) ;
    virtual ~SetMemberLists        (void) ;

  protected:

    QPoint dragPoint ;

    virtual bool FocusIn           (void) ;
    virtual void Configure         (void) ;

    virtual void setCheckable      (QTreeWidgetItem * item) ;

    virtual bool        hasItem    (void);
    virtual bool        startDrag  (QMouseEvent * event);
    virtual bool        fetchDrag  (QMouseEvent * event);
    virtual QMimeData * dragMime   (void);
    virtual void        dragDone   (Qt::DropAction dropIt,QMimeData * mime);
    virtual bool        finishDrag (QMouseEvent * event);

    virtual void run         (int Type,ThreadData * data) ;
    virtual bool Load        (ThreadData * data) ;

  private:

  public slots:

    virtual bool List              (void) ;

    virtual void New               (void) ;
    virtual void Delete            (void) ;
    virtual void Paste             (void) ;
    virtual void Export            (void) ;
    virtual void Copy              (void) ;
    virtual void SelectNone        (void) ;
    virtual void SelectAll         (void) ;

  protected slots:

    virtual bool Menu              (QPoint pos) ;

    virtual void doubleClicked     (QTreeWidgetItem * item,int column) ;

    void removeOldItem             (void) ;
    void nameFinished              (void) ;
    void identifierFinished        (void) ;
    void Ending                    (QTreeWidgetItem * item) ;

  private slots:

  signals:

    void Sets                      (SUID member) ;

} ;

class Q_MATHWIDGETS_EXPORT SetLists : public TreeDock
                                    , public CodeManager
{
  Q_OBJECT
  public:

    explicit SetLists              (StandardConstructor) ;
    virtual ~SetLists              (void) ;

  protected:

    QPoint dragPoint ;

    virtual bool FocusIn           (void) ;
    virtual void Configure         (void) ;

    virtual void setCheckable      (QTreeWidgetItem * item) ;

    virtual bool        hasItem    (void);
    virtual bool        startDrag  (QMouseEvent * event);
    virtual bool        fetchDrag  (QMouseEvent * event);
    virtual QMimeData * dragMime   (void);
    virtual void        dragDone   (Qt::DropAction dropIt,QMimeData * mime);
    virtual bool        finishDrag (QMouseEvent * event);

    virtual bool acceptDrop        (QWidget * source,const QMimeData * mime);
    virtual bool dropNew           (QWidget * source,const QMimeData * mime,QPoint pos);
    virtual bool dropMembers       (QWidget * source,QPoint pos,const UUIDs & Uuids) ;

    virtual void run         (int Type,ThreadData * data) ;
    virtual bool Load        (ThreadData * data) ;

  private:

  public slots:

    virtual bool List              (void) ;

    virtual void New               (void) ;
    virtual void Delete            (void) ;
    virtual void Paste             (void) ;
    virtual void Export            (void) ;
    virtual void Copy              (void) ;
    virtual void SelectNone        (void) ;
    virtual void SelectAll         (void) ;

  protected slots:

    virtual bool Menu              (QPoint pos) ;

    virtual void doubleClicked     (QTreeWidgetItem * item,int column) ;

    void removeOldItem             (void) ;
    void nameFinished              (void) ;
    void TypeChanged               (int index) ;
    void Ending                    (QTreeWidgetItem * item) ;

  private slots:

  signals:

    void Sets                      (SUID member) ;

} ;

class Q_MATHWIDGETS_EXPORT SetsRules : public TreeDock
                                     , public Object
                                     , public Relation
{
  Q_OBJECT
  public:

    explicit SetsRules              (StandardConstructor) ;
    virtual ~SetsRules              (void) ;

  protected:

    QPoint dragPoint ;

    virtual bool FocusIn            (void) ;
    virtual void Configure          (void) ;

    virtual bool        hasItem     (void) ;
    virtual bool        startDrag   (QMouseEvent * event) ;
    virtual bool        fetchDrag   (QMouseEvent * event) ;
    virtual QMimeData * dragMime    (void) ;
    virtual void        dragDone    (Qt::DropAction dropIt,QMimeData * mime) ;
    virtual bool        finishDrag  (QMouseEvent * event) ;

    virtual bool        acceptDrop  (QWidget * source,const QMimeData * mime);
    virtual bool        dropNew     (QWidget * source,const QMimeData * mime,QPoint pos);
    virtual bool        dropMoving  (QWidget * source,const QMimeData * mime,QPoint pos);
    virtual bool        dropAppend  (QWidget * source,const QMimeData * mime,QPoint pos);

    virtual QString     MimeType    (const QMimeData * mime);
    virtual UUIDs       MimeUuids   (const QMimeData * mime,QString mimetype) ;

    virtual void run         (int Type,ThreadData * data) ;
    virtual bool Load        (ThreadData * data) ;

  private:

  public slots:

    virtual bool startup            (void) ;
    virtual void Insert             (void) ;

  protected slots:

    virtual bool Menu               (QPoint pos) ;
    virtual void doubleClicked      (QTreeWidgetItem * item,int column) ;
    virtual void nameFinished       (void) ;
    virtual void identifierFinished (void) ;
    virtual void ruleFinished       (void) ;

  private slots:

  signals:

} ;

class Q_MATHWIDGETS_EXPORT NumSeries : public TreeWidget
                                     , public Object
{
  Q_OBJECT
  public:

    QString    Name        ;
    int        ValueTypeId ;
    QByteArray Body        ;

    explicit NumSeries         (StandardConstructor) ;
    virtual ~NumSeries         (void);

  protected:

    int Precision ;
    int Radix     ;

    virtual bool FocusIn       (void) ;
    virtual void Configure     (void) ;

    virtual void run         (int Type,ThreadData * data) ;
    virtual bool Load        (ThreadData * data) ;

  private:

  public slots:

    virtual bool startup       (void) ;
    virtual void Paste         (void) ;
    virtual void Save          (void) ;
    virtual void Resize        (void) ;
    virtual void ChangeType    (void) ;
    virtual void setPrecision  (void) ;
    virtual void setRadix      (void) ;
    virtual void toCpp         (void) ;
    virtual void ListItems     (void) ;

    virtual void Paste         (QString & text) ;

  protected slots:

    virtual bool Menu          (QPoint pos) ;
    virtual void doubleClicked (QTreeWidgetItem * item,int column) ;
    virtual void nameFinished  (void) ;

  private slots:

  signals:

} ;

class Q_MATHWIDGETS_EXPORT RuleBases : public TreeWidget
                                     , public Ownership
                                     , public GroupItems
{
  Q_OBJECT
  public:

    int           RuleType ;
    Qt::SortOrder Sorting  ;

    explicit      RuleBases     (StandardConstructor) ;
    virtual      ~RuleBases     (void) ;

    virtual QSize sizeHint      (void) const ;

  protected:

    NAMEs RuleNames ;

    virtual bool  FocusIn       (void) ;
    virtual void  Configure     (void) ;

    virtual UUIDs Rules         (SqlConnection & Connection) ;

    virtual SUID  assureItem    (SqlConnection & SC     ,
                                 SUID            uuid   ,
                                 QString         name ) ;

    virtual void  run           (int Type,ThreadData * data) ;

  private:

  public slots:

    virtual bool  startup       (void) ;
    virtual bool  List          (void) ;
    virtual void  Paste         (void) ;
    virtual void  Paste         (QString text) ;
    virtual void  Insert        (void) ;
    virtual void  Delete        (void) ;

  protected slots:

    virtual bool  Menu          (QPoint pos) ;
    virtual void  doubleClicked (QTreeWidgetItem * item,int column) ;
    virtual void  nameFinished  (void) ;
    virtual void  typeFinished  (int index) ;

  private slots:

  signals:

    void Edit                   (SUID uuid,int type) ;

} ;

class Q_MATHWIDGETS_EXPORT DecisionLists : public TreeDock
                                         , public Ownership
                                         , public GroupItems
{
  Q_OBJECT
  public:

    int           DecisionType ;
    Qt::SortOrder Sorting      ;

    explicit            DecisionLists (StandardConstructor) ;
    virtual            ~DecisionLists (void) ;

    virtual QSize       sizeHint      (void) const ;

  protected:

    QPoint dragPoint  ;
    bool   dropAction ;

    virtual void        Configure     (void) ;
    virtual bool        FocusIn       (void) ;

    virtual bool        hasItem       (void) ;
    virtual bool        startDrag     (QMouseEvent * event) ;
    virtual bool        fetchDrag     (QMouseEvent * event) ;
    virtual QMimeData * dragMime      (void) ;
    virtual void        dragDone      (Qt::DropAction dropIt,QMimeData * mime) ;
    virtual bool        finishDrag    (QMouseEvent * event) ;

    virtual SUID        assureItem    (SqlConnection & SC     ,
                                       SUID            uuid   ,
                                       QString         name ) ;

    virtual void        run           (int Type,ThreadData * data) ;

  private:

  public slots:

    virtual bool        startup       (void) ;
    virtual void        List          (void) ;
    virtual void        Insert        (void) ;
    virtual void        Rename        (void) ;
    virtual void        Copy          (void) ;
    virtual void        Delete        (void) ;
    virtual void        Paste         (void) ;
    virtual void        Paste         (QString text) ;
    virtual void        Export        (void) ;

  protected slots:

    bool                Menu          (QPoint pos) ;
    virtual void        doubleClicked (QTreeWidgetItem *item, int column) ;

  private slots:

    virtual void        NameFinished  (void) ;

  signals:

    void                Update        (QWidget * widget,SUID uuid) ;
    void                Edit          (QWidget * widget,QString name,SUID uuid) ;

} ;

class Q_MATHWIDGETS_EXPORT DecisionEditor : public TableWidget
                                          , public Ownership
                                          , public CodeManager
{
  Q_OBJECT
  public:

    SUID    Uuid          ;
    QString Name          ;
    bool    Changed       ;
    int     Conditions    ;
    int     Actions       ;
    int     MaxConditions ;

    explicit DecisionEditor       (StandardConstructor) ;
    virtual ~DecisionEditor       (void) ;

    QSize PreferSize (void) ;

  protected:

    QComboBox * Items ;
    int         AtRow ;

    virtual void contextMenuEvent (QContextMenuEvent * event) ;

    virtual bool FocusIn          (void) ;
    virtual void Configure        (void) ;

    void Dimension                (int conditions,int actions);
    void Inserted                 (void);
    void Mount                    (int row,int type) ;

    virtual void run              (int Type,ThreadData * data) ;

  private:

  public slots:

    bool startup                  (SUID uuid) ;

    void AdjustSize               (void) ;

    bool Save                     (void) ;
    void Delete                   (void) ;
    void DeleteCondition          (void) ;
    void DeleteAction             (void) ;
    void ClearActions             (void) ;

  protected slots:

    virtual bool Menu             (QPoint pos) ;

    bool Save                     (int row,int column) ;
    void CellChanged              (int row,int column) ;
    void CellPressed              (int row,int column) ;
    void CellDoubleClicked        (int row,int column) ;

    void ModifyCondition          (int index) ;
    void AddCondition             (int index) ;
    void ModifyAction             (int index) ;
    void AddAction                (int index) ;

  private slots:

  signals:

    void Updated                  (QWidget * widget) ;

} ;

class Q_MATHWIDGETS_EXPORT ActionLists : public TreeDock
                                       , public Ownership
                                       , public GroupItems
{
  Q_OBJECT
  public:

    Qt::SortOrder Sorting ;

    explicit     ActionLists       (StandardConstructor) ;
    virtual     ~ActionLists       (void) ;

    virtual QSize sizeHint         (void) const ;

  protected:

    QPoint dragPoint  ;
    bool   dropAction ;
    NAMEs  Tnames     ;

    virtual void        Configure  (void) ;
    virtual bool        FocusIn    (void) ;

    virtual bool        hasItem    (void);
    virtual bool        startDrag  (QMouseEvent * event);
    virtual bool        fetchDrag  (QMouseEvent * event);
    virtual QMimeData * dragMime   (void);
    virtual void        dragDone   (Qt::DropAction dropIt,QMimeData * mime);
    virtual bool        finishDrag (QMouseEvent * event);

    virtual SUID        assureItem (SqlConnection & SC     ,
                                    SUID            uuid   ,
                                    QString         name ) ;

    virtual void        run        (int Type,ThreadData * data) ;
    virtual bool        Load       (ThreadData * data) ;

  private:

  public slots:

    virtual bool startup           (void) ;
    virtual void List              (void) ;
    virtual void Insert            (void) ;
    virtual void Rename            (void) ;
    virtual void Copy              (void) ;
    virtual void Delete            (void) ;
    virtual void Paste             (void) ;
    virtual void Paste             (QString text) ;
    virtual void Export            (void) ;

  protected slots:

    bool         Menu              (QPoint pos) ;
    virtual void doubleClicked     (QTreeWidgetItem * item,int column) ;
    virtual void changeType        (QTreeWidgetItem * item) ;

  private slots:

    virtual void NameFinished      (void) ;
    virtual void typeFinished      (int index) ;

  signals:

    void         Update            (QWidget * widget,SUID uuid,int type) ;
    void         Edit              (QWidget * widget,QString name,SUID uuid,int type) ;

} ;

class Q_MATHWIDGETS_EXPORT ConditionLists : public TreeDock
                                          , public Ownership
                                          , public GroupItems
{
  Q_OBJECT
  public:

    Qt::SortOrder Sorting ;

    explicit            ConditionLists   (StandardConstructor) ;
    virtual            ~ConditionLists   (void) ;

    virtual QSize       sizeHint         (void) const ;

  protected:

    QPoint dragPoint  ;
    bool   dropAction ;
    NAMEs  TypeNames  ;

    virtual void        Configure        (void) ;
    virtual bool        FocusIn          (void) ;

    virtual bool        hasItem          (void) ;
    virtual bool        startDrag        (QMouseEvent * event) ;
    virtual bool        fetchDrag        (QMouseEvent * event) ;
    virtual QMimeData * dragMime         (void) ;
    virtual void        dragDone         (Qt::DropAction dropIt,QMimeData * mime) ;
    virtual bool        finishDrag       (QMouseEvent * event) ;

    virtual SUID        assureItem       (SqlConnection & SC     ,
                                          SUID            uuid   ,
                                          QString         name ) ;

    virtual void        run              (int Type,ThreadData * data) ;
    virtual bool        Load             (ThreadData * data) ;

  private:

  public slots:

    virtual bool        startup          (void) ;
    virtual void        List             (void) ;
    virtual void        Insert           (void) ;
    virtual void        Rename           (void) ;
    virtual void        Copy             (void) ;
    virtual void        Delete           (void) ;
    virtual void        Paste            (void) ;
    virtual void        Paste            (QString text) ;
    virtual void        Export           (void) ;

  protected slots:

    virtual bool        Menu             (QPoint pos) ;
    virtual void        doubleClicked    (QTreeWidgetItem *item, int column) ;

    virtual void        NameFinished     (void) ;
    virtual void        typeChanged      (int type) ;
    virtual void        identFinished    (void) ;
    virtual void        valueModified    (void) ;
    virtual void        indicatorClicked (QTreeWidgetItem *item, int column) ;
    virtual void        memberModified   (void) ;

  private slots:

  signals:

    void                Update           (QWidget * widget,SUID uuid,int type) ;
    void                Edit             (QWidget * widget,QString name,SUID uuid,int type) ;

} ;

class Q_MATHWIDGETS_EXPORT MatrixEditor : public TableWidget
                                        , public Object
{
  Q_OBJECT
  public:

    QString  name   ;
    Matrix * matrix ;

    explicit MatrixEditor  ( QWidget * parent = NULL, Plan * plan = NULL ) ;
    virtual ~MatrixEditor  ( void ) ;

  protected:

    virtual void Configure (void) ;

  private:

  public slots:

    virtual bool startup   (void) ;
    virtual void Update    (void) ;
    virtual void setHeader (bool visible) ;

  protected slots:

    void valueChanged      (int row,int column) ;

  private slots:

  signals:

} ;

class Q_MATHWIDGETS_EXPORT EditMatrix : public Widget
                                      , public Object
                                      , public SceneManager
{
  Q_OBJECT
  public:

    Matrix matrix ;

    explicit EditMatrix          (StandardConstructor) ;
    virtual ~EditMatrix          (void);

  protected:

    MatrixEditor    * Editor ;
    Ui::nEditMatrix * ui     ;

    virtual void Configure       (void) ;

    virtual void resizeEvent     (QResizeEvent * event) ;
    virtual void showEvent       (QShowEvent * event) ;

  private:

  public slots:

    virtual bool startup         (void) ;
    virtual bool load            (SUID uuid) ;
    virtual void Relocation      (void) ;
    virtual void Save            (void) ;

  protected slots:

    virtual void canSave         (void) ;
    virtual void nameChanged     (QString text) ;
    virtual void positionChanged (int state) ;
    virtual void dimChanged      (int) ;

  private slots:

  signals:

    void Updated                 (SUID uuid) ;

} ;

class Q_MATHWIDGETS_EXPORT MatricesLists : public TreeDock
                                         , public SceneManager
{
  Q_OBJECT
  public:

    explicit MatricesLists     (StandardConstructor) ;
    virtual ~MatricesLists     (void) ;

    virtual QSize sizeHint     (void) const ;

  protected:

    NAMEs Types ;

    virtual bool FocusIn       (void) ;
    virtual void Configure     (void) ;

    virtual void run           (int Type,ThreadData * data) ;
    virtual bool Load          (ThreadData * data) ;

  private:

  public slots:

    virtual bool startup       (void) ;
    virtual void Insert        (void) ;
    virtual void Updated       (SUID uuid) ;

  protected slots:

    virtual void doubleClicked (QTreeWidgetItem * item,int column) ;
    virtual bool Menu          (QPoint pos) ;

  private slots:

  signals:

    void Add                   (QWidget * widget) ;
    void Edit                  (QWidget * widget,SUID uuid) ;

} ;

}

QT_END_NAMESPACE

#endif

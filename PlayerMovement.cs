using System.Collections;
using UnityEngine;

public class PlayerMovement : MonoBehaviour
{
    private Rigidbody2D playerRigidbody;
    private CapsuleCollider2D playerCollider;
    private Animator animator;

    [Header("Running")]
    [SerializeField] float acceleration = 5f;
    [SerializeField] float maxSpeed = 2f;
    private Vector2 movementDirection;

    [Header("Jumping")]
    [SerializeField] float jumpForce = 2f;
    private bool isJumpingNextFixedUpdate = false;
    private bool canDoubleJump = false;
    [Tooltip("The delay in seconds after which the player is not able to jump anymore after losing contact to the ground.")]
    [SerializeField] float coyoteTime = 0.5f;
    [Tooltip("The gravity that is applied whilst the player is falling.")]
    [SerializeField] float fallingGravity = 2f;
    private float originalGravity;

    [Header("States")]
    [Tooltip("The size of the detection box that is used for the ground check. The player collider width is automatically added.")]
    [SerializeField] Vector2 groundDetectionBox = new Vector2(0.3f, 0.5f);
    private bool isDelayRoutineRunning = false;
    private bool isGrounded;
    private bool isFalling;

    void Awake()
    {
        playerCollider = GetComponent<CapsuleCollider2D>();
        playerRigidbody = GetComponent<Rigidbody2D>();
        animator = GetComponent<Animator>();
    }

    void Start()
    {
        originalGravity = playerRigidbody.gravityScale;
    }

    void Update()
    {
        // Running
        movementDirection = new Vector2(Input.GetAxisRaw("Horizontal"), 0);
        // flip player based on movement direction
        if (playerRigidbody.velocity.x > 0) 
            transform.localScale = new Vector3(Mathf.Abs(transform.localScale.x), transform.localScale.y);
        else if (playerRigidbody.velocity.x < 0)
            transform.localScale = new Vector3(-Mathf.Abs(transform.localScale.x), transform.localScale.y);

        if (movementDirection.x != 0) animator.SetBool("isRunning", true);
        else animator.SetBool("isRunning", false);


        // Jumping
        if (Input.GetButtonDown("Jump") && (isGrounded || canDoubleJump))
        {
            if (!isGrounded) canDoubleJump = false;
            isJumpingNextFixedUpdate = true;
        }


        // Ground check
        Vector2 groundCheckBoxPosition = playerCollider.bounds.center - Vector3.up * playerCollider.bounds.extents.y;
        Vector2 groundCheckBoxSize = new Vector2(playerCollider.bounds.size.x + groundDetectionBox.x, groundDetectionBox.y);
        Collider2D feetCollision = Physics2D.OverlapBox(groundCheckBoxPosition, groundCheckBoxSize, 0, ~LayerMask.GetMask("Player"));
        if (feetCollision != null && feetCollision.CompareTag("Ground"))
        {
            canDoubleJump = true;
            isGrounded = true;
        }
        else if (!isDelayRoutineRunning)
        {
            StartCoroutine(DelayIsGrounded(false, coyoteTime));
        }
        animator.SetBool("isGrounded", isGrounded);



        // Falling
        if (playerRigidbody.velocity.y < 0) isFalling = true;
        else isFalling = false;
        animator.SetBool("isFalling", isFalling);
    }

    private void FixedUpdate()
    {
        // Running
        playerRigidbody.AddForce(movementDirection * acceleration * Time.deltaTime);
        // set player velocity to maxSpeed if player speed surpasses maxSpeed
        if (Mathf.Abs(playerRigidbody.velocity.x) > maxSpeed)
            playerRigidbody.velocity = new Vector2(maxSpeed * Mathf.Sign(playerRigidbody.velocity.x), playerRigidbody.velocity.y);

        // Jumping
        if (isJumpingNextFixedUpdate)
        {
            isJumpingNextFixedUpdate = false;
            playerRigidbody.velocity = new Vector2(playerRigidbody.velocity.x, 0);
            playerRigidbody.AddForce(Vector2.up * jumpForce, ForceMode2D.Impulse);
        }

        // Falling
        // player falls faster than it gains height
        if (isFalling) playerRigidbody.gravityScale = fallingGravity;
        else playerRigidbody.gravityScale = originalGravity;
    }

    private IEnumerator DelayIsGrounded(bool value, float delay)
    {
        isDelayRoutineRunning = true;
        yield return new WaitForSeconds(delay);
        isGrounded = value;
        isDelayRoutineRunning = false;
    }

}
